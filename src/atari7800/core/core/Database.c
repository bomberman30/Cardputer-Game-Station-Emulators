/* ----------------------------------------------------------------------------
 *   ___  ___  ___  ___       ___  ____  ___  _  _
 *  /__/ /__/ /  / /__  /__/ /__    /   /_   / |/ /
 * /    / \  /__/ ___/ ___/ ___/   /   /__  /    /  emulator
 *
 * ----------------------------------------------------------------------------
 * Copyright 2005 Greg Stanton
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 * ----------------------------------------------------------------------------
 * Database.c
 * ----------------------------------------------------------------------------
 */
#include "Database.h"
#include "Cartridge.h"

void database_Initialize(void) { }

#ifdef A7800_NO_DATABASE

void database_Load(const char *digest)
{
   (void)digest;
}

#else

typedef struct cartridge_db
{
   char digest[33];
   uint8_t type;
   bool pokey;
   uint8_t controller1;
   uint8_t controller2;
   uint8_t region;
   uint32_t flags;
} cartridge_db_t;

static const struct cartridge_db db_list[] = 
{
   {
      "4332c24e4f3bc72e7fe1b77adf66c2b7",         /* digest */
      0,                                          /* type */
      false,                                      /* pokey */
      1,                                          /* controller 1 */
      1,                                          /* controller 2 */
      0,                                          /* region */
      0,                                          /* flags */
   },
   {
      "0be996d25144966d5541c9eb4919b289",         /* digest */
      4,                                          /* type */
      false,                                      /* pokey */
      1,                                          /* controller 1 */
      1,                                          /* controller 2 */
      0,                                          /* region */
      0,                                          /* flags */
   },
   {
      "aadde920b3aaba03bc10b40bd0619c94",         /* digest */
      4,                                          /* type */
      false,                                      /* pokey */
      1,                                          /* controller 1 */
      1,                                          /* controller 2 */
      1,                                          /* region */
      0,                                          /* flags */
   },
   {
      "877dcc97a775ed55081864b2dbf5f1e2",         /* digest */
      2,                                          /* type */
      false,                                      /* pokey */
      3,                                          /* controller 1 */
      3,                                          /* controller 2 */
      0,                                          /* region */
      0,                                          /* flags */
   },
   {
      "de3e9496cb7341f865f27e5a72c7f2f5",         /* digest */
      2,                                          /* type */
      false,                                      /* pokey */
      3,                                          /* controller 1 */
      3,                                          /* controller 2 */
      1,                                          /* region */
      0,                                          /* flags */
   },
   {
      "404f95103b70975a42cb09946dc3adca",         /* digest */
      3,                                          /* type */
      true,                                       /* pokey */
      1,                                          /* controller 1 */
      1,                                          /* controller 2 */
      0,                                          /* region */
      0,                                          /* flags */
   },
   {
      "07342c78619ba6ffcc61c10e907e3b50",         /* digest */
      0,                                          /* type */
      false,                                      /* pokey */
      1,                                          /* controller 1 */
      1,                                          /* controller 2 */
      0,                                          /* region */
      0,                                          /* flags */
   },
   {
      "8fc3a695eaea3984912d98ed4a543376",         /* digest */
      0,                                          /* type */
      true,                                       /* pokey */
      1,                                          /* controller 1 */
      1,                                          /* controller 2 */
      0,                                          /* region */
      0,                                          /* flags */
   },
   {
      "b558814d54904ce0582e2f6a801d03af",         /* digest */
      0,                                          /* type */
      true,                                       /* pokey */
      1,                                          /* controller 1 */
      1,                                          /* controller 2 */
      1,                                          /* region */
      0,                                          /* flags */
   },
   {
      "42682415906c21c6af80e4198403ffda",         /* digest */
      1,                                          /* type */
      true,                                       /* pokey */
      2,                                          /* controller 1 */
      1,                                          /* controller 2 */
      0,                                          /* region */
      0,                                          /* flags */
   },
   {
      "babe2bc2976688bafb8b23c192658126",         /* digest */
      1,                                          /* type */
      true,                                       /* pokey */
      2,                                          /* controller 1 */
      1,                                          /* controller 2 */
      1,                                          /* region */
      0,                                          /* flags */
   },
   {
      "f5f6b69c5eb4b55fc163158d1a6b423e",         /* digest */
      4,                                          /* type */
      false,                                      /* pokey */
      1,                                          /* controller 1 */
      1,                                          /* controller 2 */
      0,                                          /* region */
      1,                                          /* flags */
   },
   {
      "fba002089fcfa176454ab507e0eb76cb",         /* digest */
      4,                                          /* type */
      false,                                      /* pokey */
      1,                                          /* controller 1 */
      1,                                          /* controller 2 */
      1,                                          /* region */
      1,                                          /* flags */
   },
   {
      "386bded4a944bae455fedf56206dd1dd",         /* digest */
      0,                                          /* type */
      false,                                      /* pokey */
      1,                                          /* controller 1 */
      1,                                          /* controller 2 */
      1,                                          /* region */
      0,                                          /* flags */
   },
   {
      "aefb78276913e8a166e460222e378fec",         /* digest */
      0,                                          /* type */
      true,                                       /* pokey */
      1,                                          /* controller 1 */
      1,                                          /* controller 2 */
      0,                                          /* region */
      0,                                          /* flags */
   },
   {
      "5a09946e57dbe30408a8f253a28d07db",         /* digest */
      0,                                          /* type */
      false,                                      /* pokey */
      1,                                          /* controller 1 */
      1,                                          /* controller 2 */
      0,                                          /* region */
      0,                                          /* flags */
   },
   {
      "38c056a48472d9a9e16ebda5ed91dae7",         /* digest */
      0,                                          /* type */
      false,                                      /* pokey */
      1,                                          /* controller 1 */
      1,                                          /* controller 2 */
      1,                                          /* region */
      0,                                          /* flags */
   },
   {
      "93e4387864b014c155d7c17877990d1e",         /* digest */
      0,                                          /* type */
      false,                                      /* pokey */
      1,                                          /* controller 1 */
      1,                                          /* controller 2 */
      0,                                          /* region */
      0,                                          /* flags */
   },
   {
      "59d4edb0230b5acc918b94f6bc94779f",         /* digest */
      0,                                          /* type */
      false,                                      /* pokey */
      1,                                          /* controller 1 */
      1,                                          /* controller 2 */
      1,                                          /* region */
      0,                                          /* flags */
   },
   {
      "2e8e28f6ad8b9b9267d518d880c73ebb",         /* digest */
      1,                                          /* type */
      true,                                       /* pokey */
      1,                                          /* controller 1 */
      1,                                          /* controller 2 */
      0,                                          /* region */
      0,                                          /* flags */
   },
   {
      "55da6c6c3974d013f517e725aa60f48e",         /* digest */
      1,                                          /* type */
      true,                                       /* pokey */
      1,                                          /* controller 1 */
      1,                                          /* controller 2 */
      1,                                          /* region */
      0,                                          /* flags */
   },
   {
      "db691469128d9a4217ec7e315930b646",         /* digest */
      1,                                          /* type */
      false,                                      /* pokey */
      1,                                          /* controller 1 */
      1,                                          /* controller 2 */
      0,                                          /* region */
      0,                                          /* flags */
   },
   {
      "7cbe78fa06f47ba6516a67a4b003c9ee",         /* digest */
      1,                                          /* type */
      false,                                      /* pokey */
      1,                                          /* controller 1 */
      1,                                          /* controller 2 */
      1,                                          /* region */
      0,                                          /* flags */
   },
   {
      "a94e4560b6ad053a1c24e096f1262ebf",         /* digest */
      2,                                          /* type */
      false,                                      /* pokey */
      3,                                          /* controller 1 */
      3,                                          /* controller 2 */
      0,                                          /* region */
      0,                                          /* flags */
   },
   {
      "63db371d67a98daec547b2abd5e7aa95",         /* digest */
      2,                                          /* type */
      false,                                      /* pokey */
      3,                                          /* controller 1 */
      3,                                          /* controller 2 */
      1,                                          /* region */
      0,                                          /* flags */
   },
   {
      "179b76ff729d4849b8f66a502398acae",         /* digest */
      1,                                          /* type */
      false,                                      /* pokey */
      1,                                          /* controller 1 */
      1,                                          /* controller 2 */
      0,                                          /* region */
      0,                                          /* flags */
   },
   {
      "a2b8e2f159642c4b91de82e9a2928494",         /* digest */
      1,                                          /* type */
      false,                                      /* pokey */
      1,                                          /* controller 1 */
      1,                                          /* controller 2 */
      1,                                          /* region */
      0,                                          /* flags */
   },
   {
      "95ac811c7d27af0032ba090f28c107bd",         /* digest */
      0,                                          /* type */
      false,                                      /* pokey */
      1,                                          /* controller 1 */
      1,                                          /* controller 2 */
      0,                                          /* region */
      0,                                          /* flags */
   },
   {
      "2d5d99b993a885b063f9f22ce5e6523d",         /* digest */
      0,                                          /* type */
      false,                                      /* pokey */
      1,                                          /* controller 1 */
      1,                                          /* controller 2 */
      1,                                          /* region */
      0,                                          /* flags */
   },
   {
      "731879ea82fc0ca245e39e036fe293e6",         /* digest */
      0,                                          /* type */
      false,                                      /* pokey */
      1,                                          /* controller 1 */
      1,                                          /* controller 2 */
      0,                                          /* region */
      0,                                          /* flags */
   },
   {
      "408dca9fc40e2b5d805f403fa0509436",         /* digest */
      0,                                          /* type */
      false,                                      /* pokey */
      1,                                          /* controller 1 */
      1,                                          /* controller 2 */
      1,                                          /* region */
      0,                                          /* flags */
   },
   {
      "5e332fbfc1e0fc74223d2e73271ce650",         /* digest */
      0,                                          /* type */
      false,                                      /* pokey */
      1,                                          /* controller 1 */
      1,                                          /* controller 2 */
      0,                                          /* region */
      0,                                          /* flags */
   },
   {
      "4dc5f88243250461bd61053b13777060",         /* digest */
      0,                                          /* type */
      false,                                      /* pokey */
      1,                                          /* controller 1 */
      1,                                          /* controller 2 */
      1,                                          /* region */
      0,                                          /* flags */
   },
   {
      "19f1ee292a23636bd57d408b62de79c7",         /* digest */
      0,                                          /* type */
      false,                                      /* pokey */
      1,                                          /* controller 1 */
      1,                                          /* controller 2 */
      0,                                          /* region */
      0,                                          /* flags */
   },
   {
      "8e96ef14ce9b5d84bcbc996b66d6d4c7",         /* digest */
      0,                                          /* type */
      false,                                      /* pokey */
      1,                                          /* controller 1 */
      1,                                          /* controller 2 */
      1,                                          /* region */
      0,                                          /* flags */
   },
   {
      "543484c00ba233736bcaba2da20eeea9",         /* digest */
      6,                                          /* type */
      false,                                      /* pokey */
      1,                                          /* controller 1 */
      1,                                          /* controller 2 */
      0,                                          /* region */
      0,                                          /* flags */
   },
   {
      "de2ebafcf0e37aaa9d0e9525a7f4dd62",         /* digest */
      6,                                          /* type */
      false,                                      /* pokey */
      1,                                          /* controller 1 */
      1,                                          /* controller 2 */
      1,                                          /* region */
      0,                                          /* flags */
   },
   {
      "cb6f27c7bcbe65b60595d6f54bd39348",         /* digest */
      6,                                          /* type */
      false,                                      /* pokey */
      1,                                          /* controller 1 */
      1,                                          /* controller 2 */
      0,                                          /* region */
      0,                                          /* flags */
   },
   {
      "59ca19c4b024fdfcd25b75ac836c7749",         /* digest */
      6,                                          /* type */
      false,                                      /* pokey */
      1,                                          /* controller 1 */
      1,                                          /* controller 2 */
      0,                                          /* region */
      0,                                          /* flags */
   },
   {
      "098b209aac126f2c2edbc982df09cd1b",         /* digest */
      6,                                          /* type */
      false,                                      /* pokey */
      1,                                          /* controller 1 */
      1,                                          /* controller 2 */
      0,                                          /* region */
      0,                                          /* flags */
   },
   {
      "2251a6a0f3aec84cc0aff66fc9fa91e8",         /* digest */
      5,                                          /* type */
      false,                                      /* pokey */
      1,                                          /* controller 1 */
      1,                                          /* controller 2 */
      0,                                          /* region */
      0,                                          /* flags */
   },
   {
      "e7709da8e49d3767301947a0a0b9d2e6",         /* digest */
      5,                                          /* type */
      false,                                      /* pokey */
      1,                                          /* controller 1 */
      1,                                          /* controller 2 */
      1,                                          /* region */
      0,                                          /* flags */
   },
   {
      "d25d5d19188e9f149977c49eb0367cd1",         /* digest */
      4,                                          /* type */
      false,                                      /* pokey */
      1,                                          /* controller 1 */
      1,                                          /* controller 2 */
      0,                                          /* region */
      0,                                          /* flags */
   },
   {
      "23505651ac2e47f3637152066c3aa62f",         /* digest */
      4,                                          /* type */
      false,                                      /* pokey */
      1,                                          /* controller 1 */
      1,                                          /* controller 2 */
      1,                                          /* region */
      0,                                          /* flags */
   },
   {
      "07dbbfe612a0a28e283c01545e59f25e",         /* digest */
      4,                                          /* type */
      false,                                      /* pokey */
      1,                                          /* controller 1 */
      1,                                          /* controller 2 */
      0,                                          /* region */
      0,                                          /* flags */
   },
   {
      "e80f24e953563e6b61556737d67d3836",         /* digest */
      4,                                          /* type */
      false,                                      /* pokey */
      1,                                          /* controller 1 */
      1,                                          /* controller 2 */
      1,                                          /* region */
      0,                                          /* flags */
   },
   {
      "cf76b00244105b8e03cdc37677ec1073",         /* digest */
      0,                                          /* type */
      false,                                      /* pokey */
      1,                                          /* controller 1 */
      1,                                          /* controller 2 */
      0,                                          /* region */
      0,                                          /* flags */
   },
   {
      "de0d4f5a9bf1c1bddee3ed2f7ec51209",         /* digest */
      0,                                          /* type */
      false,                                      /* pokey */
      1,                                          /* controller 1 */
      1,                                          /* controller 2 */
      1,                                          /* region */
      0,                                          /* flags */
   },
   {
      "fb8d803b328b2e442548f7799cfa9a4a",         /* digest */
      0,                                          /* type */
      false,                                      /* pokey */
      1,                                          /* controller 1 */
      1,                                          /* controller 2 */
      0,                                          /* region */
      0,                                          /* flags */
   },
   {
      "f5dc7dc8e38072d3d65bd90a660148ce",         /* digest */
      0,                                          /* type */
      false,                                      /* pokey */
      1,                                          /* controller 1 */
      1,                                          /* controller 2 */
      1,                                          /* region */
      0,                                          /* flags */
   },
   {
      "06204dadc975be5e5e37e7cc66f984cf",         /* digest */
      0,                                          /* type */
      false,                                      /* pokey */
      1,                                          /* controller 1 */
      1,                                          /* controller 2 */
      0,                                          /* region */
      0,                                          /* flags */
   },
   {
      "fd9e78e201b6baafddfd3e1fbfe6ba31",         /* digest */
      0,                                          /* type */
      false,                                      /* pokey */
      1,                                          /* controller 1 */
      1,                                          /* controller 2 */
      0,                                          /* region */
      0,                                          /* flags */
   },
   {
      "0baec96787ce17f390e204de1a136e59",         /* digest */
      0,                                          /* type */
      false,                                      /* pokey */
      1,                                          /* controller 1 */
      1,                                          /* controller 2 */
      1,                                          /* region */
      0,                                          /* flags */
   },
   {
      "c3672482ca93f70eafd9134b936c3feb",         /* digest */
      4,                                          /* type */
      false,                                      /* pokey */
      1,                                          /* controller 1 */
      1,                                          /* controller 2 */
      0,                                          /* region */
      0,                                          /* flags */
   },
   {
      "8c2c2a1ea6e9a928a44c3151ba5c1ce3",         /* digest */
      4,                                          /* type */
      false,                                      /* pokey */
      1,                                          /* controller 1 */
      1,                                          /* controller 2 */
      1,                                          /* region */
      0,                                          /* flags */
   },
   {
      "baebc9246c087e893dfa489632157180",         /* digest */
      3,                                          /* type */
      false,                                      /* pokey */
      1,                                          /* controller 1 */
      1,                                          /* controller 2 */
      0,                                          /* region */
      0,                                          /* flags */
   },
   {
      "80dead01ea2db5045f6f4443faa6fce8",         /* digest */
      3,                                          /* type */
      false,                                      /* pokey */
      1,                                          /* controller 1 */
      1,                                          /* controller 2 */
      1,                                          /* region */
      0,                                          /* flags */
   },
   {
      "045fd12050b7f2b842d5970f2414e912",         /* digest */
      3,                                          /* type */
      false,                                      /* pokey */
      1,                                          /* controller 1 */
      1,                                          /* controller 2 */
      0,                                          /* region */
      0,                                          /* flags */
   },
   {
      "dfb86f4d06f05ad00cf418f0a59a24f7",         /* digest */
      3,                                          /* type */
      false,                                      /* pokey */
      1,                                          /* controller 1 */
      1,                                          /* controller 2 */
      1,                                          /* region */
      0,                                          /* flags */
   },
   {
      "f18b3b897a25ab3885b43b4bd141b396",         /* digest */
      0,                                          /* type */
      false,                                      /* pokey */
      1,                                          /* controller 1 */
      1,                                          /* controller 2 */
      0,                                          /* region */
      0,                                          /* flags */
   },
   {
      "f2dae0264a4b4a73762b9d7177e989f6",         /* digest */
      0,                                          /* type */
      false,                                      /* pokey */
      1,                                          /* controller 1 */
      1,                                          /* controller 2 */
      1,                                          /* region */
      0,                                          /* flags */
   },
   {
      "c3a5a8692a423d43d9d28dd5b7d109d9",         /* digest */
      0,                                          /* type */
      false,                                      /* pokey */
      1,                                          /* controller 1 */
      1,                                          /* controller 2 */
      0,                                          /* region */
      0,                                          /* flags */
   },
   {
      "5e0a1e832bbcea6facb832fde23a440a",         /* digest */
      4,                                          /* type */
      false,                                      /* pokey */
      1,                                          /* controller 1 */
      1,                                          /* controller 2 */
      1,                                          /* region */
      0,                                          /* flags */
   },
   {
      "17b3b764d33eae9b5260f01df7bb9d2f",         /* digest */
      4,                                          /* type */
      false,                                      /* pokey */
      1,                                          /* controller 1 */
      1,                                          /* controller 2 */
      0,                                          /* region */
      0,                                          /* flags */
   },
   {
      "f57d0af323d4e173fb49ed447f0563d7",         /* digest */
      0,                                          /* type */
      false,                                      /* pokey */
      1,                                          /* controller 1 */
      1,                                          /* controller 2 */
      0,                                          /* region */
      2,                                          /* flags */
   },
   {
      "2931b75811ad03f3ac9330838f3d231b",         /* digest */
      0,                                          /* type */
      false,                                      /* pokey */
      1,                                          /* controller 1 */
      1,                                          /* controller 2 */
      1,                                          /* region */
      0,                                          /* flags */
   },
   {
      "41b1dfe4174ad5434f1ef1aeeac90592",         /* digest */
      0,                                          /* type */
      false,                                      /* pokey */
      1,                                          /* controller 1 */
      1,                                          /* controller 2 */
      0,                                          /* region */
      2,                                          /* flags */
   },
   {
      "4fc0b16e3a5c463790a04fb42a1e4c40",         /* digest */
      0,                                          /* type */
      false,                                      /* pokey */
      1,                                          /* controller 1 */
      1,                                          /* controller 2 */
      0,                                          /* region */
      2,                                          /* flags */
   },
   {
      "431ca060201ee1f9eb49d44962874049",         /* digest */
      0,                                          /* type */
      false,                                      /* pokey */
      1,                                          /* controller 1 */
      1,                                          /* controller 2 */
      0,                                          /* region */
      0,                                          /* flags */
   },
   {
      "d2e861306be78e44248bb71d7475d8a3",         /* digest */
      0,                                          /* type */
      false,                                      /* pokey */
      1,                                          /* controller 1 */
      1,                                          /* controller 2 */
      1,                                          /* region */
      0,                                          /* flags */
   },
   {
      "37b5692e33a98115e574185fa8398c22",         /* digest */
      4,                                          /* type */
      false,                                      /* pokey */
      1,                                          /* controller 1 */
      1,                                          /* controller 2 */
      0,                                          /* region */
      0,                                          /* flags */
   },
   {
      "6819c37b96063b024898a19dbae2df54",         /* digest */
      4,                                          /* type */
      false,                                      /* pokey */
      1,                                          /* controller 1 */
      1,                                          /* controller 2 */
      1,                                          /* region */
      0,                                          /* flags */
   },
   {
      "f2f5e5841e4dda89a2faf8933dc33ea6",         /* digest */
      4,                                          /* type */
      false,                                      /* pokey */
      1,                                          /* controller 1 */
      1,                                          /* controller 2 */
      0,                                          /* region */
      0,                                          /* flags */
   },
   {
      "2e9dbad6c0fa381a6cd1bb9abf98a104",         /* digest */
      4,                                          /* type */
      false,                                      /* pokey */
      1,                                          /* controller 1 */
      1,                                          /* controller 2 */
      1,                                          /* region */
      0,                                          /* flags */
   },
   {
      "bedc30ec43587e0c98fc38c39c1ef9d0",         /* digest */
      4,                                          /* type */
      false,                                      /* pokey */
      2,                                          /* controller 1 */
      2,                                          /* controller 2 */
      0,                                          /* region */
      0,                                          /* flags */
   },
   {
      "c80155d7eec9e3dcb79aa6b83c9ccd1e",         /* digest */
      4,                                          /* type */
      false,                                      /* pokey */
      2,                                          /* controller 1 */
      2,                                          /* controller 2 */
      1,                                          /* region */
      0,                                          /* flags */
   },
   {
      "bc1e905db1008493a9632aa83ab4682b",         /* digest */
      4,                                          /* type */
      false,                                      /* pokey */
      1,                                          /* controller 1 */
      1,                                          /* controller 2 */
      0,                                          /* region */
      0,                                          /* flags */
   },
   {
      "6794ea31570eba0b88a0bf1ead3f3f1b",         /* digest */
      4,                                          /* type */
      false,                                      /* pokey */
      1,                                          /* controller 1 */
      1,                                          /* controller 2 */
      1,                                          /* region */
      0,                                          /* flags */
   },
   {
      "017066f522908081ec3ee624f5e4a8aa",         /* digest */
      2,                                          /* type */
      false,                                      /* pokey */
      1,                                          /* controller 1 */
      1,                                          /* controller 2 */
      0,                                          /* region */
      3,                                          /* flags */
   },
   {
      "3bc8f554cf86f8132a623cc2201a564b",         /* digest */
      4,                                          /* type */
      false,                                      /* pokey */
      1,                                          /* controller 1 */
      1,                                          /* controller 2 */
      0,                                          /* region */
      0,                                          /* flags */
   },
   {
      "5330bfe428a6b601b7e76c2cfc4cd049",         /* digest */
      4,                                          /* type */
      false,                                      /* pokey */
      1,                                          /* controller 1 */
      1,                                          /* controller 2 */
      1,                                          /* region */
      0,                                          /* flags */
   },
   {
      "fc0ea52a9fac557251b65ee680d951e5",         /* digest */
      0,                                          /* type */
      false,                                      /* pokey */
      1,                                          /* controller 1 */
      1,                                          /* controller 2 */
      0,                                          /* region */
      0,                                          /* flags */
   },
   {
      "56469e8c5ff8983c6cb8dadc64eb0363",         /* digest */
      0,                                          /* type */
      false,                                      /* pokey */
      1,                                          /* controller 1 */
      1,                                          /* controller 2 */
      1,                                          /* region */
      0,                                          /* flags */
   },
   {
      "220121f771fc4b98cef97dc040e8d378",         /* digest */
      4,                                          /* type */
      false,                                      /* pokey */
      1,                                          /* controller 1 */
      1,                                          /* controller 2 */
      0,                                          /* region */
      0,                                          /* flags */
   },
   {
      "ea0c859aa54fe5eaf4c1f327fab06221",         /* digest */
      4,                                          /* type */
      false,                                      /* pokey */
      1,                                          /* controller 1 */
      1,                                          /* controller 2 */
      1,                                          /* region */
      0,                                          /* flags */
   },
   {
      "74569571a208f8b0b1ccfb22d7c914e1",         /* digest */
      0,                                          /* type */
      false,                                      /* pokey */
      1,                                          /* controller 1 */
      1,                                          /* controller 2 */
      0,                                          /* region */
      1,                                          /* flags */
   },
   {
      "8dba0425f0262e5704581d8757a1a6e3",         /* digest */
      0,                                          /* type */
      false,                                      /* pokey */
      1,                                          /* controller 1 */
      1,                                          /* controller 2 */
      1,                                          /* region */
      1,                                          /* flags */
   },
   {
      "1a5207870dec6fae9111cb747e20d8e3",         /* digest */
      0,                                          /* type */
      false,                                      /* pokey */
      1,                                          /* controller 1 */
      1,                                          /* controller 2 */
      0,                                          /* region */
      0,                                          /* flags */
   },
   {
      "ec206c8db4316eb1ebce9fc960da7d8f",         /* digest */
      4,                                          /* type */
      false,                                      /* pokey */
      1,                                          /* controller 1 */
      1,                                          /* controller 2 */
      0,                                          /* region */
      0,                                          /* flags */
   },
   {
      "05f43244465943ce819780a71a5b572a",         /* digest */
      4,                                          /* type */
      false,                                      /* pokey */
      1,                                          /* controller 1 */
      1,                                          /* controller 2 */
      0,                                          /* region */
      0,                                          /* flags */
   },
   {
      "33aea1e2b6634a1dec8c7006d9afda22",         /* digest */
      4,                                          /* type */
      false,                                      /* pokey */
      1,                                          /* controller 1 */
      1,                                          /* controller 2 */
      0,                                          /* region */
      0,                                          /* flags */
   },
   {
      "2837a8fd49b7fc7ccd70fd45b69c5099",         /* digest */
      4,                                          /* type */
      false,                                      /* pokey */
      1,                                          /* controller 1 */
      1,                                          /* controller 2 */
      1,                                          /* region */
      0,                                          /* flags */
   },
   {
      "86546808dc60961cdb1b20e761c50ab1",         /* digest */
      3,                                          /* type */
      false,                                      /* pokey */
      1,                                          /* controller 1 */
      1,                                          /* controller 2 */
      0,                                          /* region */
      0,                                          /* flags */
   },
   {
      "584582bb09ee8122e7fc09dc7d1ed813",         /* digest */
      0,                                          /* type */
      false,                                      /* pokey */
      1,                                          /* controller 1 */
      1,                                          /* controller 2 */
      0,                                          /* region */
      0,                                          /* flags */
   },
   {
      "865457e0e0f48253b08f77b9e18f93b2",         /* digest */
      0,                                          /* type */
      false,                                      /* pokey */
      1,                                          /* controller 1 */
      1,                                          /* controller 2 */
      1,                                          /* region */
      0,                                          /* flags */
   },
   {
      "1745feadabb24e7cefc375904c73fa4c",         /* digest */
      3,                                          /* type */
      false,                                      /* pokey */
      1,                                          /* controller 1 */
      1,                                          /* controller 2 */
      0,                                          /* region */
      0,                                          /* flags */
   },
   {
      "ac03806cef2558fc795a7d5d8dba7bc0",         /* digest */
      6,                                          /* type */
      false,                                      /* pokey */
      1,                                          /* controller 1 */
      1,                                          /* controller 2 */
      0,                                          /* region */
      0,                                          /* flags */
   },
   {
      "442761655bb25ddfe5f7ab16bf591c6f",         /* digest */
      1,                                          /* type */
      false,                                      /* pokey */
      1,                                          /* controller 1 */
      1,                                          /* controller 2 */
      0,                                          /* region */
      0,                                          /* flags */
   },
   {
      "bfad016d6e77eaccec74c0340aded8b9",         /* digest */
      1,                                          /* type */
      false,                                      /* pokey */
      1,                                          /* controller 1 */
      1,                                          /* controller 2 */
      0,                                          /* region */
      0,                                          /* flags */
   },
   {
      "383ed9bd1efb9b6cb3388a777678c928",         /* digest */
      1,                                          /* type */
      false,                                      /* pokey */
      1,                                          /* controller 1 */
      1,                                          /* controller 2 */
      0,                                          /* region */
      0,                                          /* flags */
   },
   {
      "8f7eb10ad0bd75474abf0c6c36c08486",         /* digest */
      0,                                          /* type */
      false,                                      /* pokey */
      1,                                          /* controller 1 */
      1,                                          /* controller 2 */
      0,                                          /* region */
      0,                                          /* flags */
   },
   {
      "b3bc889e9cc498636990c5a4d980e85c",         /* digest */
      7,                                          /* type */
      false,                                      /* pokey */
      1,                                          /* controller 1 */
      1,                                          /* controller 2 */
      0,                                          /* region */
      0,                                          /* flags */
   },
   {
      "66ecaafe1b82ae68ffc96267aaf7a4d7",         /* digest */
      0,                                          /* type */
      false,                                      /* pokey */
      1,                                          /* controller 1 */
      1,                                          /* controller 2 */
      0,                                          /* region */
      0,                                          /* flags */
   },
   {
      "980c35ae9625773a450aa7ef51751c04",         /* digest */
      4,                                          /* type */
      false,                                      /* pokey */
      1,                                          /* controller 1 */
      1,                                          /* controller 2 */
      0,                                          /* region */
      0,                                          /* flags */
   },
   {
      "53db322c201323fe2ca8f074c0a2bf86",         /* digest */
      4,                                          /* type */
      false,                                      /* pokey */
      1,                                          /* controller 1 */
      1,                                          /* controller 2 */
      1,                                          /* region */
      0,                                          /* flags */
   },
   {
      "b697d9c2d1b9f6cb21041286d1bbfa7f",         /* digest */
      4,                                          /* type */
      false,                                      /* pokey */
      2,                                          /* controller 1 */
      2,                                          /* controller 2 */
      0,                                          /* region */
      0,                                          /* flags */
   },
   {
      "5469b4de0608f23a5c4f98f331c9e75f",         /* digest */
      4,                                          /* type */
      false,                                      /* pokey */
      2,                                          /* controller 1 */
      2,                                          /* controller 2 */
      1,                                          /* region */
      0,                                          /* flags */
   },
   {
      "2d643ac548c40e58c99d0fe433ba4ba0",         /* digest */
      3,                                          /* type */
      false,                                      /* pokey */
      2,                                          /* controller 1 */
      2,                                          /* controller 2 */
      0,                                          /* region */
      0,                                          /* flags */
   },
   {
      "cbb0746192540a13b4c7775c7ce2021f",         /* digest */
      3,                                          /* type */
      false,                                      /* pokey */
      1,                                          /* controller 1 */
      1,                                          /* controller 2 */
      0,                                          /* region */
      0,                                          /* flags */
   },
   {
      "cc18e3b37a507c4217eb6cb1de8c8538",         /* digest */
      0,                                          /* type */
      false,                                      /* pokey */
      1,                                          /* controller 1 */
      1,                                          /* controller 2 */
      0,                                          /* region */
      0,                                          /* flags */
   },
   {
      "162f9c953f0657689cc74ab20b40280f",         /* digest */
      0,                                          /* type */
      false,                                      /* pokey */
      1,                                          /* controller 1 */
      1,                                          /* controller 2 */
      1,                                          /* region */
      0,                                          /* flags */
   },
   {
      "59b5793bece1c80f77b55d60fb39cb94",         /* digest */
      0,                                          /* type */
      false,                                      /* pokey */
      1,                                          /* controller 1 */
      1,                                          /* controller 2 */
      0,                                          /* region */
      0,                                          /* flags */
   },
   {
      "95d7c321dce8f57623a9c5b4947bb375",         /* digest */
      0,                                          /* type */
      false,                                      /* pokey */
      1,                                          /* controller 1 */
      1,                                          /* controller 2 */
      1,                                          /* region */
      0,                                          /* flags */
   },
   {
      "44f862bca77d68b56b32534eda5c198d",         /* digest */
      1,                                          /* type */
      false,                                      /* pokey */
      1,                                          /* controller 1 */
      1,                                          /* controller 2 */
      0,                                          /* region */
      0,                                          /* flags */
   },
   {
      "5c4f752371a523f15e9980fea73b874d",         /* digest */
      1,                                          /* type */
      false,                                      /* pokey */
      1,                                          /* controller 1 */
      1,                                          /* controller 2 */
      0,                                          /* region */
      0,                                          /* flags */
   },
   {
      "1af475ff6429a160752b592f0f92b287",         /* digest */
      0,                                          /* type */
      false,                                      /* pokey */
      1,                                          /* controller 1 */
      1,                                          /* controller 2 */
      0,                                          /* region */
      0,                                          /* flags */
   },
   {
      "3bb9c8d9adc912dd7f8471c97445cd8d",         /* digest */
      0,                                          /* type */
      false,                                      /* pokey */
      1,                                          /* controller 1 */
      1,                                          /* controller 2 */
      1,                                          /* region */
      0,                                          /* flags */
   },
   {
      "c3903ab01a51222a52197dbfe6538ecf",         /* digest */
      0,                                          /* type */
      false,                                      /* pokey */
      1,                                          /* controller 1 */
      1,                                          /* controller 2 */
      0,                                          /* region */
      0,                                          /* flags */
   },
   {
      "682338364243b023ecc9d24f0abfc9a7",         /* digest */
      0,                                          /* type */
      false,                                      /* pokey */
      1,                                          /* controller 1 */
      1,                                          /* controller 2 */
      1,                                          /* region */
      0,                                          /* flags */
   },
   {
      "208ef955fa90a29815eb097bce89bace",         /* digest */
      4,                                          /* type */
      false,                                      /* pokey */
      1,                                          /* controller 1 */
      1,                                          /* controller 2 */
      0,                                          /* region */
      0,                                          /* flags */
   },
   {
      "d12e665347f354048b9d13092f7868c9",         /* digest */
      3,                                          /* type */
      false,                                      /* pokey */
      1,                                          /* controller 1 */
      1,                                          /* controller 2 */
      0,                                          /* region */
      0,                                          /* flags */
   },
   {
      "8d64763db3100aadc552db5e6868506a",         /* digest */
      3,                                          /* type */
      false,                                      /* pokey */
      1,                                          /* controller 1 */
      1,                                          /* controller 2 */
      0,                                          /* region */
      0,                                          /* flags */
   },
   {
      "32a37244a9c6cc928dcdf02b45365aa8",         /* digest */
      3,                                          /* type */
      false,                                      /* pokey */
      1,                                          /* controller 1 */
      1,                                          /* controller 2 */
      1,                                          /* region */
      0,                                          /* flags */
   },
   {
      "acf63758ecf3f3dd03e9d654ae6b69b7",         /* digest */
      1,                                          /* type */
      false,                                      /* pokey */
      1,                                          /* controller 1 */
      1,                                          /* controller 2 */
      0,                                          /* region */
      0,                                          /* flags */
   },
   {
      "427cb05d0a1abb068998e2760d77f4fb",         /* digest */
      1,                                          /* type */
      false,                                      /* pokey */
      1,                                          /* controller 1 */
      1,                                          /* controller 2 */
      0,                                          /* region */
      0,                                          /* flags */
   },
   {
      "3799d72f78dda2ee87b0ef8bf7b91186",         /* digest */
      3,                                          /* type */
      false,                                      /* pokey */
      1,                                          /* controller 1 */
      1,                                          /* controller 2 */
      0,                                          /* region */
      0,                                          /* flags */
   },
   {
      "05fb699db9eef564e2fe45c568746dbc",         /* digest */
      4,                                          /* type */
      false,                                      /* pokey */
      1,                                          /* controller 1 */
      1,                                          /* controller 2 */
      0,                                          /* region */
      0,                                          /* flags */
   },
   {
      "70937c3184f0be33d06f7f4382ca54de",         /* digest */
      4,                                          /* type */
      false,                                      /* pokey */
      1,                                          /* controller 1 */
      1,                                          /* controller 2 */
      1,                                          /* region */
      0,                                          /* flags */
   },
   {
      "d7dc17379aa25e5ae3c14b9e780c6f6d",         /* digest */
      0,                                          /* type */
      false,                                      /* pokey */
      1,                                          /* controller 1 */
      1,                                          /* controller 2 */
      0,                                          /* region */
      0,                                          /* flags */
   },
   {
      "b1a9f196ce5f47ca8caf8fa7bc4ca46c",         /* digest */
      0,                                          /* type */
      false,                                      /* pokey */
      1,                                          /* controller 1 */
      1,                                          /* controller 2 */
      1,                                          /* region */
      0,                                          /* flags */
   }
};

void database_Load(const char *digest)
{
   unsigned i;
   size_t len = sizeof(db_list) / sizeof(db_list[0]);

   for (i = 0; i < len; i++)
   {
      if (!strcmp(db_list[i].digest, digest))
      {
         cartridge_type          = db_list[i].type;
         cartridge_pokey         = db_list[i].pokey;
         cartridge_controller[0] = db_list[i].controller1;
         cartridge_controller[1] = db_list[i].controller2;
         cartridge_region        = db_list[i].region;
         cartridge_flags         = db_list[i].flags;
         return;
      }
   }
}

#endif
