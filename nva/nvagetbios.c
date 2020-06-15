/*
 * Copyright (C) 2011 Martin Peres <martin.peres@ensi-bourges.fr>
 * Copyright (C) 2011 Marcelina Kościelnicka <mwk@0x04.net>
 * Copyright (C) 2011 Emil Velikov <emil.l.velikov@gmail.com>
 * All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice (including the next
 * paragraph) shall be included in all copies or substantial portions of the
 * Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR
 * OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
 * ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 */

#include "nva.h"
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>

#define NV_PRAMIN_OFFSET            0x00700000
#define NV_PROM_OFFSET              0x00300000
#define NV01_PROM_SIZE              0x00008000
#define NV03_PROM_SIZE              0x00010000
#define NV_PROM_SIZE                0x00100000

#define EOK 1
#define EUNK 0
#define ECRC -1
#define ESIG -2
#define ECARD -3

static int nv_cksum(const uint8_t *data, unsigned int length)
{
	/*
	 * There's a few checksums in the BIOS, so here's a generic checking
	 * function.
	 */
	int i;
	uint8_t sum = 0;

	for (i = 0; i < length; i++)
		sum += data[i];

	if (sum) {
		fprintf(stderr, "Invalid checksum. Broken vbios or broken retrieval method?\n");
		return ECRC;
	}
	return EOK;
}

static int nv_checksignature(const uint8_t *data)
{
	uint16_t pcir_ptr;

	/* bail if no rom signature */
	if (data[0] != 0x55 || data[1] != 0xaa) {
		fprintf(stderr, "Invalid signature(0x55aa). You may want to try another retrieval method.\n");
		return ESIG;
	}

	/* additional check (see note below) - read PCI record header */
	pcir_ptr = data[0x18] | data[0x19] << 8;
	if (data[pcir_ptr] != 'P' ||
	    data[pcir_ptr + 1] != 'C' ||
	    data[pcir_ptr + 2] != 'I' ||
	    data[pcir_ptr + 3] != 'R') {

		fprintf(stderr, "Invalid signature(PCIR). You may want to try another retrieval method.\n");
		return ESIG;
	}

	return EOK;
}

static int nv_ckbios(const uint8_t *data)
{
	uint16_t pcir_ptr, pcir_ptr2;
	uint32_t ret = EUNK;
	int vbios_len, vbios2_len;

	ret = nv_checksignature(data);
	if (ret != EOK)
		return ret;

	pcir_ptr = data[0x18] | data[0x19] << 8;
	vbios_len = data[pcir_ptr + 0x10] * 512 + data[pcir_ptr + 0x11] * 512 * 256;
	ret = nv_cksum(data, vbios_len);

	/* Check for a second vbios */
	if (!(data[pcir_ptr + 0x15] & 0x80)) {

		fprintf(stderr, "Card has second bios\n");

		ret = nv_checksignature(data + vbios_len);
		if (ret != EOK)
			return ret;

		pcir_ptr2 = data[vbios_len + 0x18] | data[vbios_len + 0x19] << 8;
		vbios2_len = data[vbios_len + pcir_ptr2 + 0x10] * 512 + data[vbios_len + pcir_ptr2 + 0x11] * 512 * 256;

		ret = nv_cksum(data + vbios_len, vbios2_len);
	}
	return ret;
}

/* vbios should at least be NV_PROM_SIZE bytes long */
int vbios_extract_prom(int cnum, uint8_t *vbios, int *length)
{
	uint32_t pci_cfg_50 = 0;
	uint32_t ret = EUNK;
	int i;

	fprintf(stderr, "Attempt to extract the vbios from card %i (nv%02x) using PROM.\n",
			cnum, nva_cards[cnum]->chipset.chipset);

	int32_t prom_offset;
	int32_t prom_size;
	int32_t pbus_offset = 0;

	if (nva_cards[cnum]->chipset.chipset < 0x03) {
		prom_offset = 0x610000;
		prom_size = NV01_PROM_SIZE;
	} else if (nva_cards[cnum]->chipset.chipset < 0x04) {
		prom_offset = 0x110000;
		prom_size = NV03_PROM_SIZE;
	} else {
		if (nva_cards[cnum]->chipset.chipset < 0x40)
			pbus_offset = 0x1800;
		else
			pbus_offset = 0x88000;
		prom_offset = 0x300000;
		prom_size = NV_PROM_SIZE;
		pci_cfg_50 = nva_rd32(cnum, pbus_offset + 0x50);
		nva_wr32(cnum, pbus_offset + 0x50, 0);
	}
	
	/* on some 6600GT/6800LE prom reads are messed up.  nvclock alleges a
	 * a good read may be obtained by waiting or re-reading (cargocult: 5x)
	 * each byte.  we'll hope pramin has something usable instead
	 */

	for (i = 0; i < prom_size; i++)
		vbios[i] = nva_rd32(cnum, prom_offset + (i & ~3)) >> (i & 3) * 8;

	ret = nv_ckbios(vbios);
	if (nva_cards[cnum]->chipset.chipset >= 0x04) {
		nva_wr32(cnum, pbus_offset + 0x50, pci_cfg_50);
	}
	*length = prom_size;
	return ret;
}

/* vbios should at least be NV_PROM_SIZE bytes long */
int vbios_extract_pramin(int cnum, uint8_t *vbios, int *length)
{
	uint32_t old_bar0_pramin = 0;
	uint32_t ret = EUNK;
	int i;

	if (nva_cards[cnum]->chipset.chipset < 0x04) {
		fprintf(stderr, "Card %i (nv%02x) does not support PRAMIN!\n",
				cnum, nva_cards[cnum]->chipset.chipset);
		return ECARD;
	}

	fprintf(stderr, "Attempt to extract the vbios from card %i (nv%02x) using PRAMIN.\n",
			cnum, nva_cards[cnum]->chipset.chipset);

	if (nva_cards[cnum]->chipset.card_type >= 0x50) {
		uint64_t vbios_vram = (uint64_t)(nva_rd32(cnum, 0x619f04) & ~0xff) << 8;

		if (!vbios_vram)
			vbios_vram = ((uint64_t)nva_rd32(cnum, 0x1700) << 16) + 0xf0000;

		old_bar0_pramin = nva_rd32(cnum, 0x1700);
		nva_wr32(cnum, 0x1700, vbios_vram >> 16);
		*length = 0x20000;
	} else {
		*length = 0x10000;
	}

	for (i = 0; i < *length; i++)
		vbios[i] = nva_rd8(cnum, NV_PRAMIN_OFFSET + i);

	ret = nv_ckbios(vbios);
	if (nva_cards[cnum]->chipset.card_type >= 0x50)
		nva_wr32(cnum, 0x1700, old_bar0_pramin);

	return ret;
}

int32_t extract_strap_peek(int cnum)
{
	return nva_rd32(cnum, 0x101000);
}

void usage(int error_code)
{
	fprintf(stderr,
		"\n"
		"Usage: nvagetbios [-c card_number] [-s extraction_method]\n"
		"                  [-S strap_peek_out] > vbios.rom\n");
	exit(error_code);
}

int main(int argc, char **argv) {
	uint8_t vbios[NV_PROM_SIZE];
	int c;
	int cnum =0;
	char const *source = NULL;
	int result = 0;
	int length;
	FILE *strap_peek_output = stderr;
	int32_t strap_peek;

	if (nva_init()) {
		fprintf (stderr, "PCI init failure!\n");
		return 1;
	}

	/* Arguments parsing */
	while ((c = getopt (argc, argv, "hc:s:S:")) != -1) {
		switch (c) {
			case 'h':
				usage(0);
				break;
			case 'c':
				sscanf(optarg, "%d", &cnum);
				break;
			case 's':
				source = optarg;
				break;
			case 'S':
				strap_peek_output = fopen(optarg, "w+");
				if (!strap_peek_output) {
					fprintf(stderr, "-S: Error while opening %s: %s\n",
						optarg, strerror(errno));
					return 1;
				}
				break;
		}
	}

	if (cnum >= nva_cardsnum) {
		if (nva_cardsnum)
			fprintf (stderr, "No such card.\n");
		else
			fprintf (stderr, "No cards found.\n");
		return 1;
	}

	/* Extraction */
	if (!source) {
		fprintf(stderr, "No extraction method specified (using -s extraction_method). Autodetecting.\n");
		if (nva_cards[cnum]->chipset.chipset < 4) {
			result = vbios_extract_prom(cnum, vbios, &length);
		} else {
			result = vbios_extract_pramin(cnum, vbios, &length);
			if (result != EOK)
				result = vbios_extract_prom(cnum, vbios, &length);
		}

		if (result != EOK) {
			fprintf(stderr, "Autodetection failed, aborting.\n");
			return 1;
		}
	}
	else if (strcasecmp(source, "pramin") == 0) {
		result = vbios_extract_pramin(cnum, vbios, &length);
		if (result == ESIG)
			fprintf(stderr, "You may want to run nvagetbios -s prom instead!\n");
	}
	else if (strcasecmp(source, "prom") == 0)
		result = vbios_extract_prom(cnum, vbios, &length);
	else {
		fprintf(stderr, "Unknown vbios extraction method.\n");
		usage(1);
	}

	if (isatty(fileno(stdout))) {
		fprintf(stderr, "Refusing to write BIOS image to a terminal - please redirect output to a file.\n");
	} else {
		/* print the vbios on stdout */
		fwrite(vbios, 1, length, stdout);
	}

	strap_peek = extract_strap_peek(cnum);
	if (isatty(fileno(strap_peek_output))) {
		fprintf(strap_peek_output,
			"Your strap peek is: 0x%x\n"
			"Please save this value in the same directory as your vbios as a file named strap_peek (or use -S)\n",
			strap_peek);
	} else {
		fprintf(strap_peek_output, "0x%x", strap_peek);
	}

	if (result == EOK)
		fprintf(stderr, "Vbios extracted successfully!\n");
	else if (result == ESIG)
		fprintf(stderr, "Vbios extracted but it may be corrupted!\n");

	return result != EOK;
}
