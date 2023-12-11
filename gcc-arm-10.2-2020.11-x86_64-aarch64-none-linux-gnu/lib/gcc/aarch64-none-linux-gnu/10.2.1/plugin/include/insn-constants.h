/* Generated automatically by the program `genconstants'
   from the machine description file `md'.  */

#ifndef GCC_INSN_CONSTANTS_H
#define GCC_INSN_CONSTANTS_H

#define V24_REGNUM 56
#define SVE_STRICT_GP 1
#define SP_REGNUM 31
#define R5_REGNUM 5
#define V6_REGNUM 38
#define R8_REGNUM 8
#define V2_REGNUM 34
#define R12_REGNUM 12
#define P4_REGNUM 72
#define P3_REGNUM 71
#define P7_REGNUM 75
#define R10_REGNUM 10
#define V15_REGNUM 47
#define R24_REGNUM 24
#define SVE_RELAXED_GP 0
#define R16_REGNUM 16
#define R0_REGNUM 0
#define SPECULATION_SCRATCH_REGNUM 14
#define IP0_REGNUM 16
#define V5_REGNUM 37
#define V4_REGNUM 36
#define V8_REGNUM 40
#define R15_REGNUM 15
#define R26_REGNUM 26
#define V18_REGNUM 50
#define R27_REGNUM 27
#define P11_REGNUM 79
#define SFP_REGNUM 64
#define EP0_REGNUM 12
#define V12_REGNUM 44
#define V31_REGNUM 63
#define FP_REGNUM 29
#define V0_REGNUM 32
#define R3_REGNUM 3
#define R30_REGNUM 30
#define P2_REGNUM 70
#define V10_REGNUM 42
#define R22_REGNUM 22
#define V22_REGNUM 54
#define R19_REGNUM 19
#define SVE_MAYBE_NOT_PTRUE 0
#define V25_REGNUM 57
#define R6_REGNUM 6
#define R9_REGNUM 9
#define STACK_CLASH_SVE_CFA_REGNUM 11
#define V3_REGNUM 35
#define R13_REGNUM 13
#define P8_REGNUM 76
#define V16_REGNUM 48
#define R25_REGNUM 25
#define LAST_SAVED_REGNUM 83
#define V26_REGNUM 58
#define V20_REGNUM 52
#define V28_REGNUM 60
#define R1_REGNUM 1
#define EP1_REGNUM 13
#define SVE_KNOWN_PTRUE 1
#define IP1_REGNUM 17
#define AP_REGNUM 65
#define P14_REGNUM 82
#define P0_REGNUM 68
#define FFRT_REGNUM 85
#define R28_REGNUM 28
#define P12_REGNUM 80
#define R4_REGNUM 4
#define SPECULATION_TRACKER_REGNUM 15
#define R7_REGNUM 7
#define V1_REGNUM 33
#define V19_REGNUM 51
#define V11_REGNUM 43
#define R20_REGNUM 20
#define VG_REGNUM 67
#define V27_REGNUM 59
#define V14_REGNUM 46
#define R23_REGNUM 23
#define V9_REGNUM 41
#define P15_REGNUM 83
#define V23_REGNUM 55
#define LR_REGNUM 30
#define CC_REGNUM 66
#define R14_REGNUM 14
#define P6_REGNUM 74
#define P9_REGNUM 77
#define V17_REGNUM 49
#define P10_REGNUM 78
#define V7_REGNUM 39
#define V29_REGNUM 61
#define R2_REGNUM 2
#define V30_REGNUM 62
#define R11_REGNUM 11
#define V13_REGNUM 45
#define R17_REGNUM 17
#define FFR_REGNUM 84
#define R18_REGNUM 18
#define P1_REGNUM 69
#define P5_REGNUM 73
#define R21_REGNUM 21
#define R29_REGNUM 29
#define P13_REGNUM 81
#define V21_REGNUM 53

enum arches {
  ARCHES_ANY = 0,
  ARCHES_RCPC8_4 = 1,
  ARCHES_FP = 2,
  ARCHES_SIMD = 3,
  ARCHES_SVE = 4,
  ARCHES_FP16 = 5
};
#define NUM_ARCHES_VALUES 6
extern const char *const arches_strings[];

enum unspec {
  UNSPEC_AUTIA1716 = 0,
  UNSPEC_AUTIB1716 = 1,
  UNSPEC_AUTIASP = 2,
  UNSPEC_AUTIBSP = 3,
  UNSPEC_CALLEE_ABI = 4,
  UNSPEC_CASESI = 5,
  UNSPEC_CRC32B = 6,
  UNSPEC_CRC32CB = 7,
  UNSPEC_CRC32CH = 8,
  UNSPEC_CRC32CW = 9,
  UNSPEC_CRC32CX = 10,
  UNSPEC_CRC32H = 11,
  UNSPEC_CRC32W = 12,
  UNSPEC_CRC32X = 13,
  UNSPEC_FCVTZS = 14,
  UNSPEC_FCVTZU = 15,
  UNSPEC_FJCVTZS = 16,
  UNSPEC_FRINT32Z = 17,
  UNSPEC_FRINT32X = 18,
  UNSPEC_FRINT64Z = 19,
  UNSPEC_FRINT64X = 20,
  UNSPEC_URECPE = 21,
  UNSPEC_FRECPE = 22,
  UNSPEC_FRECPS = 23,
  UNSPEC_FRECPX = 24,
  UNSPEC_FRINTA = 25,
  UNSPEC_FRINTI = 26,
  UNSPEC_FRINTM = 27,
  UNSPEC_FRINTN = 28,
  UNSPEC_FRINTP = 29,
  UNSPEC_FRINTX = 30,
  UNSPEC_FRINTZ = 31,
  UNSPEC_GOTSMALLPIC = 32,
  UNSPEC_GOTSMALLPIC28K = 33,
  UNSPEC_GOTSMALLTLS = 34,
  UNSPEC_GOTTINYPIC = 35,
  UNSPEC_GOTTINYTLS = 36,
  UNSPEC_LD1 = 37,
  UNSPEC_LD2 = 38,
  UNSPEC_LD2_DREG = 39,
  UNSPEC_LD2_DUP = 40,
  UNSPEC_LD3 = 41,
  UNSPEC_LD3_DREG = 42,
  UNSPEC_LD3_DUP = 43,
  UNSPEC_LD4 = 44,
  UNSPEC_LD4_DREG = 45,
  UNSPEC_LD4_DUP = 46,
  UNSPEC_LD2_LANE = 47,
  UNSPEC_LD3_LANE = 48,
  UNSPEC_LD4_LANE = 49,
  UNSPEC_MB = 50,
  UNSPEC_NOP = 51,
  UNSPEC_PACIA1716 = 52,
  UNSPEC_PACIB1716 = 53,
  UNSPEC_PACIASP = 54,
  UNSPEC_PACIBSP = 55,
  UNSPEC_PRLG_STK = 56,
  UNSPEC_REV = 57,
  UNSPEC_RBIT = 58,
  UNSPEC_SABAL = 59,
  UNSPEC_SABDL2 = 60,
  UNSPEC_SADALP = 61,
  UNSPEC_SCVTF = 62,
  UNSPEC_SISD_NEG = 63,
  UNSPEC_SISD_SSHL = 64,
  UNSPEC_SISD_USHL = 65,
  UNSPEC_SSHL_2S = 66,
  UNSPEC_ST1 = 67,
  UNSPEC_ST2 = 68,
  UNSPEC_ST3 = 69,
  UNSPEC_ST4 = 70,
  UNSPEC_ST2_LANE = 71,
  UNSPEC_ST3_LANE = 72,
  UNSPEC_ST4_LANE = 73,
  UNSPEC_TLS = 74,
  UNSPEC_TLSDESC = 75,
  UNSPEC_TLSLE12 = 76,
  UNSPEC_TLSLE24 = 77,
  UNSPEC_TLSLE32 = 78,
  UNSPEC_TLSLE48 = 79,
  UNSPEC_UABAL = 80,
  UNSPEC_UABDL2 = 81,
  UNSPEC_UADALP = 82,
  UNSPEC_UCVTF = 83,
  UNSPEC_USHL_2S = 84,
  UNSPEC_VSTRUCTDUMMY = 85,
  UNSPEC_SSP_SYSREG = 86,
  UNSPEC_SP_SET = 87,
  UNSPEC_SP_TEST = 88,
  UNSPEC_RSQRT = 89,
  UNSPEC_RSQRTE = 90,
  UNSPEC_RSQRTS = 91,
  UNSPEC_NZCV = 92,
  UNSPEC_XPACLRI = 93,
  UNSPEC_LD1_SVE = 94,
  UNSPEC_ST1_SVE = 95,
  UNSPEC_LDNT1_SVE = 96,
  UNSPEC_STNT1_SVE = 97,
  UNSPEC_LD1RQ = 98,
  UNSPEC_LD1_GATHER = 99,
  UNSPEC_LDFF1_GATHER = 100,
  UNSPEC_LDNT1_GATHER = 101,
  UNSPEC_ST1_SCATTER = 102,
  UNSPEC_STNT1_SCATTER = 103,
  UNSPEC_PRED_X = 104,
  UNSPEC_PRED_Z = 105,
  UNSPEC_PTEST = 106,
  UNSPEC_PTRUE = 107,
  UNSPEC_UNPACKSHI = 108,
  UNSPEC_UNPACKUHI = 109,
  UNSPEC_UNPACKSLO = 110,
  UNSPEC_UNPACKULO = 111,
  UNSPEC_PACK = 112,
  UNSPEC_WHILEGE = 113,
  UNSPEC_WHILEGT = 114,
  UNSPEC_WHILEHI = 115,
  UNSPEC_WHILEHS = 116,
  UNSPEC_WHILELE = 117,
  UNSPEC_WHILELO = 118,
  UNSPEC_WHILELS = 119,
  UNSPEC_WHILELT = 120,
  UNSPEC_WHILERW = 121,
  UNSPEC_WHILEWR = 122,
  UNSPEC_LDN = 123,
  UNSPEC_STN = 124,
  UNSPEC_INSR = 125,
  UNSPEC_CLASTA = 126,
  UNSPEC_CLASTB = 127,
  UNSPEC_FADDA = 128,
  UNSPEC_REV_SUBREG = 129,
  UNSPEC_REINTERPRET = 130,
  UNSPEC_SPECULATION_TRACKER = 131,
  UNSPEC_SPECULATION_TRACKER_REV = 132,
  UNSPEC_COPYSIGN = 133,
  UNSPEC_TTEST = 134,
  UNSPEC_UPDATE_FFR = 135,
  UNSPEC_UPDATE_FFRT = 136,
  UNSPEC_RDFFR = 137,
  UNSPEC_WRFFR = 138,
  UNSPEC_SVE_LANE_SELECT = 139,
  UNSPEC_SVE_CNT_PAT = 140,
  UNSPEC_SVE_PREFETCH = 141,
  UNSPEC_SVE_PREFETCH_GATHER = 142,
  UNSPEC_SVE_COMPACT = 143,
  UNSPEC_SVE_SPLICE = 144,
  UNSPEC_GEN_TAG = 145,
  UNSPEC_GEN_TAG_RND = 146,
  UNSPEC_TAG_SPACE = 147,
  UNSPEC_LD1RO = 148,
  UNSPEC_SALT_ADDR = 149,
  UNSPEC_ASHIFT_SIGNED = 150,
  UNSPEC_ASHIFT_UNSIGNED = 151,
  UNSPEC_ABS = 152,
  UNSPEC_FMAX = 153,
  UNSPEC_FMAXNMV = 154,
  UNSPEC_FMAXV = 155,
  UNSPEC_FMIN = 156,
  UNSPEC_FMINNMV = 157,
  UNSPEC_FMINV = 158,
  UNSPEC_FADDV = 159,
  UNSPEC_ADDV = 160,
  UNSPEC_SMAXV = 161,
  UNSPEC_SMINV = 162,
  UNSPEC_UMAXV = 163,
  UNSPEC_UMINV = 164,
  UNSPEC_SHADD = 165,
  UNSPEC_UHADD = 166,
  UNSPEC_SRHADD = 167,
  UNSPEC_URHADD = 168,
  UNSPEC_SHSUB = 169,
  UNSPEC_UHSUB = 170,
  UNSPEC_ADDHN = 171,
  UNSPEC_RADDHN = 172,
  UNSPEC_SUBHN = 173,
  UNSPEC_RSUBHN = 174,
  UNSPEC_ADDHN2 = 175,
  UNSPEC_RADDHN2 = 176,
  UNSPEC_SUBHN2 = 177,
  UNSPEC_RSUBHN2 = 178,
  UNSPEC_SQDMULH = 179,
  UNSPEC_SQRDMULH = 180,
  UNSPEC_PMUL = 181,
  UNSPEC_FMULX = 182,
  UNSPEC_USQADD = 183,
  UNSPEC_SUQADD = 184,
  UNSPEC_SQXTUN = 185,
  UNSPEC_SQXTN = 186,
  UNSPEC_UQXTN = 187,
  UNSPEC_SSRA = 188,
  UNSPEC_USRA = 189,
  UNSPEC_SRSRA = 190,
  UNSPEC_URSRA = 191,
  UNSPEC_SRSHR = 192,
  UNSPEC_URSHR = 193,
  UNSPEC_SQSHLU = 194,
  UNSPEC_SQSHL = 195,
  UNSPEC_UQSHL = 196,
  UNSPEC_SQSHRUN = 197,
  UNSPEC_SQRSHRUN = 198,
  UNSPEC_SQSHRN = 199,
  UNSPEC_UQSHRN = 200,
  UNSPEC_SQRSHRN = 201,
  UNSPEC_UQRSHRN = 202,
  UNSPEC_SSHL = 203,
  UNSPEC_USHL = 204,
  UNSPEC_SRSHL = 205,
  UNSPEC_URSHL = 206,
  UNSPEC_SQRSHL = 207,
  UNSPEC_UQRSHL = 208,
  UNSPEC_SSLI = 209,
  UNSPEC_USLI = 210,
  UNSPEC_SSRI = 211,
  UNSPEC_USRI = 212,
  UNSPEC_SSHLL = 213,
  UNSPEC_USHLL = 214,
  UNSPEC_ADDP = 215,
  UNSPEC_TBL = 216,
  UNSPEC_TBX = 217,
  UNSPEC_CONCAT = 218,
  UNSPEC_ZIP1 = 219,
  UNSPEC_ZIP2 = 220,
  UNSPEC_UZP1 = 221,
  UNSPEC_UZP2 = 222,
  UNSPEC_TRN1 = 223,
  UNSPEC_TRN2 = 224,
  UNSPEC_EXT = 225,
  UNSPEC_REV64 = 226,
  UNSPEC_REV32 = 227,
  UNSPEC_REV16 = 228,
  UNSPEC_AESE = 229,
  UNSPEC_AESD = 230,
  UNSPEC_AESMC = 231,
  UNSPEC_AESIMC = 232,
  UNSPEC_SHA1C = 233,
  UNSPEC_SHA1M = 234,
  UNSPEC_SHA1P = 235,
  UNSPEC_SHA1H = 236,
  UNSPEC_SHA1SU0 = 237,
  UNSPEC_SHA1SU1 = 238,
  UNSPEC_SHA256H = 239,
  UNSPEC_SHA256H2 = 240,
  UNSPEC_SHA256SU0 = 241,
  UNSPEC_SHA256SU1 = 242,
  UNSPEC_PMULL = 243,
  UNSPEC_PMULL2 = 244,
  UNSPEC_REV_REGLIST = 245,
  UNSPEC_VEC_SHR = 246,
  UNSPEC_SQRDMLAH = 247,
  UNSPEC_SQRDMLSH = 248,
  UNSPEC_FMAXNM = 249,
  UNSPEC_FMINNM = 250,
  UNSPEC_SDOT = 251,
  UNSPEC_UDOT = 252,
  UNSPEC_SM3SS1 = 253,
  UNSPEC_SM3TT1A = 254,
  UNSPEC_SM3TT1B = 255,
  UNSPEC_SM3TT2A = 256,
  UNSPEC_SM3TT2B = 257,
  UNSPEC_SM3PARTW1 = 258,
  UNSPEC_SM3PARTW2 = 259,
  UNSPEC_SM4E = 260,
  UNSPEC_SM4EKEY = 261,
  UNSPEC_SHA512H = 262,
  UNSPEC_SHA512H2 = 263,
  UNSPEC_SHA512SU0 = 264,
  UNSPEC_SHA512SU1 = 265,
  UNSPEC_FMLAL = 266,
  UNSPEC_FMLSL = 267,
  UNSPEC_FMLAL2 = 268,
  UNSPEC_FMLSL2 = 269,
  UNSPEC_ADR = 270,
  UNSPEC_SEL = 271,
  UNSPEC_BRKA = 272,
  UNSPEC_BRKB = 273,
  UNSPEC_BRKN = 274,
  UNSPEC_BRKPA = 275,
  UNSPEC_BRKPB = 276,
  UNSPEC_PFIRST = 277,
  UNSPEC_PNEXT = 278,
  UNSPEC_CNTP = 279,
  UNSPEC_SADDV = 280,
  UNSPEC_UADDV = 281,
  UNSPEC_ANDV = 282,
  UNSPEC_IORV = 283,
  UNSPEC_XORV = 284,
  UNSPEC_ANDF = 285,
  UNSPEC_IORF = 286,
  UNSPEC_XORF = 287,
  UNSPEC_REVB = 288,
  UNSPEC_REVH = 289,
  UNSPEC_REVW = 290,
  UNSPEC_SMUL_HIGHPART = 291,
  UNSPEC_UMUL_HIGHPART = 292,
  UNSPEC_FMLA = 293,
  UNSPEC_FMLS = 294,
  UNSPEC_FEXPA = 295,
  UNSPEC_FMMLA = 296,
  UNSPEC_FTMAD = 297,
  UNSPEC_FTSMUL = 298,
  UNSPEC_FTSSEL = 299,
  UNSPEC_SMATMUL = 300,
  UNSPEC_UMATMUL = 301,
  UNSPEC_USMATMUL = 302,
  UNSPEC_TRN1Q = 303,
  UNSPEC_TRN2Q = 304,
  UNSPEC_UZP1Q = 305,
  UNSPEC_UZP2Q = 306,
  UNSPEC_ZIP1Q = 307,
  UNSPEC_ZIP2Q = 308,
  UNSPEC_COND_CMPEQ_WIDE = 309,
  UNSPEC_COND_CMPGE_WIDE = 310,
  UNSPEC_COND_CMPGT_WIDE = 311,
  UNSPEC_COND_CMPHI_WIDE = 312,
  UNSPEC_COND_CMPHS_WIDE = 313,
  UNSPEC_COND_CMPLE_WIDE = 314,
  UNSPEC_COND_CMPLO_WIDE = 315,
  UNSPEC_COND_CMPLS_WIDE = 316,
  UNSPEC_COND_CMPLT_WIDE = 317,
  UNSPEC_COND_CMPNE_WIDE = 318,
  UNSPEC_COND_FABS = 319,
  UNSPEC_COND_FADD = 320,
  UNSPEC_COND_FCADD90 = 321,
  UNSPEC_COND_FCADD270 = 322,
  UNSPEC_COND_FCMEQ = 323,
  UNSPEC_COND_FCMGE = 324,
  UNSPEC_COND_FCMGT = 325,
  UNSPEC_COND_FCMLA = 326,
  UNSPEC_COND_FCMLA90 = 327,
  UNSPEC_COND_FCMLA180 = 328,
  UNSPEC_COND_FCMLA270 = 329,
  UNSPEC_COND_FCMLE = 330,
  UNSPEC_COND_FCMLT = 331,
  UNSPEC_COND_FCMNE = 332,
  UNSPEC_COND_FCMUO = 333,
  UNSPEC_COND_FCVT = 334,
  UNSPEC_COND_FCVTZS = 335,
  UNSPEC_COND_FCVTZU = 336,
  UNSPEC_COND_FDIV = 337,
  UNSPEC_COND_FMAX = 338,
  UNSPEC_COND_FMAXNM = 339,
  UNSPEC_COND_FMIN = 340,
  UNSPEC_COND_FMINNM = 341,
  UNSPEC_COND_FMLA = 342,
  UNSPEC_COND_FMLS = 343,
  UNSPEC_COND_FMUL = 344,
  UNSPEC_COND_FMULX = 345,
  UNSPEC_COND_FNEG = 346,
  UNSPEC_COND_FNMLA = 347,
  UNSPEC_COND_FNMLS = 348,
  UNSPEC_COND_FRECPX = 349,
  UNSPEC_COND_FRINTA = 350,
  UNSPEC_COND_FRINTI = 351,
  UNSPEC_COND_FRINTM = 352,
  UNSPEC_COND_FRINTN = 353,
  UNSPEC_COND_FRINTP = 354,
  UNSPEC_COND_FRINTX = 355,
  UNSPEC_COND_FRINTZ = 356,
  UNSPEC_COND_FSCALE = 357,
  UNSPEC_COND_FSQRT = 358,
  UNSPEC_COND_FSUB = 359,
  UNSPEC_COND_SCVTF = 360,
  UNSPEC_COND_UCVTF = 361,
  UNSPEC_LASTA = 362,
  UNSPEC_LASTB = 363,
  UNSPEC_ASHIFT_WIDE = 364,
  UNSPEC_ASHIFTRT_WIDE = 365,
  UNSPEC_LSHIFTRT_WIDE = 366,
  UNSPEC_LDFF1 = 367,
  UNSPEC_LDNF1 = 368,
  UNSPEC_FCADD90 = 369,
  UNSPEC_FCADD270 = 370,
  UNSPEC_FCMLA = 371,
  UNSPEC_FCMLA90 = 372,
  UNSPEC_FCMLA180 = 373,
  UNSPEC_FCMLA270 = 374,
  UNSPEC_ASRD = 375,
  UNSPEC_ADCLB = 376,
  UNSPEC_ADCLT = 377,
  UNSPEC_ADDHNB = 378,
  UNSPEC_ADDHNT = 379,
  UNSPEC_BDEP = 380,
  UNSPEC_BEXT = 381,
  UNSPEC_BGRP = 382,
  UNSPEC_CADD270 = 383,
  UNSPEC_CADD90 = 384,
  UNSPEC_CDOT = 385,
  UNSPEC_CDOT180 = 386,
  UNSPEC_CDOT270 = 387,
  UNSPEC_CDOT90 = 388,
  UNSPEC_CMLA = 389,
  UNSPEC_CMLA180 = 390,
  UNSPEC_CMLA270 = 391,
  UNSPEC_CMLA90 = 392,
  UNSPEC_COND_FCVTLT = 393,
  UNSPEC_COND_FCVTNT = 394,
  UNSPEC_COND_FCVTX = 395,
  UNSPEC_COND_FCVTXNT = 396,
  UNSPEC_COND_FLOGB = 397,
  UNSPEC_EORBT = 398,
  UNSPEC_EORTB = 399,
  UNSPEC_FADDP = 400,
  UNSPEC_FMAXNMP = 401,
  UNSPEC_FMAXP = 402,
  UNSPEC_FMINNMP = 403,
  UNSPEC_FMINP = 404,
  UNSPEC_FMLALB = 405,
  UNSPEC_FMLALT = 406,
  UNSPEC_FMLSLB = 407,
  UNSPEC_FMLSLT = 408,
  UNSPEC_HISTCNT = 409,
  UNSPEC_HISTSEG = 410,
  UNSPEC_MATCH = 411,
  UNSPEC_NMATCH = 412,
  UNSPEC_PMULLB = 413,
  UNSPEC_PMULLB_PAIR = 414,
  UNSPEC_PMULLT = 415,
  UNSPEC_PMULLT_PAIR = 416,
  UNSPEC_RADDHNB = 417,
  UNSPEC_RADDHNT = 418,
  UNSPEC_RSHRNB = 419,
  UNSPEC_RSHRNT = 420,
  UNSPEC_RSUBHNB = 421,
  UNSPEC_RSUBHNT = 422,
  UNSPEC_SABDLB = 423,
  UNSPEC_SABDLT = 424,
  UNSPEC_SADDLB = 425,
  UNSPEC_SADDLBT = 426,
  UNSPEC_SADDLT = 427,
  UNSPEC_SADDWB = 428,
  UNSPEC_SADDWT = 429,
  UNSPEC_SBCLB = 430,
  UNSPEC_SBCLT = 431,
  UNSPEC_SHRNB = 432,
  UNSPEC_SHRNT = 433,
  UNSPEC_SLI = 434,
  UNSPEC_SMAXP = 435,
  UNSPEC_SMINP = 436,
  UNSPEC_SMULHRS = 437,
  UNSPEC_SMULHS = 438,
  UNSPEC_SMULLB = 439,
  UNSPEC_SMULLT = 440,
  UNSPEC_SQCADD270 = 441,
  UNSPEC_SQCADD90 = 442,
  UNSPEC_SQDMULLB = 443,
  UNSPEC_SQDMULLBT = 444,
  UNSPEC_SQDMULLT = 445,
  UNSPEC_SQRDCMLAH = 446,
  UNSPEC_SQRDCMLAH180 = 447,
  UNSPEC_SQRDCMLAH270 = 448,
  UNSPEC_SQRDCMLAH90 = 449,
  UNSPEC_SQRSHRNB = 450,
  UNSPEC_SQRSHRNT = 451,
  UNSPEC_SQRSHRUNB = 452,
  UNSPEC_SQRSHRUNT = 453,
  UNSPEC_SQSHRNB = 454,
  UNSPEC_SQSHRNT = 455,
  UNSPEC_SQSHRUNB = 456,
  UNSPEC_SQSHRUNT = 457,
  UNSPEC_SQXTNB = 458,
  UNSPEC_SQXTNT = 459,
  UNSPEC_SQXTUNB = 460,
  UNSPEC_SQXTUNT = 461,
  UNSPEC_SRI = 462,
  UNSPEC_SSHLLB = 463,
  UNSPEC_SSHLLT = 464,
  UNSPEC_SSUBLB = 465,
  UNSPEC_SSUBLBT = 466,
  UNSPEC_SSUBLT = 467,
  UNSPEC_SSUBLTB = 468,
  UNSPEC_SSUBWB = 469,
  UNSPEC_SSUBWT = 470,
  UNSPEC_SUBHNB = 471,
  UNSPEC_SUBHNT = 472,
  UNSPEC_TBL2 = 473,
  UNSPEC_UABDLB = 474,
  UNSPEC_UABDLT = 475,
  UNSPEC_UADDLB = 476,
  UNSPEC_UADDLT = 477,
  UNSPEC_UADDWB = 478,
  UNSPEC_UADDWT = 479,
  UNSPEC_UMAXP = 480,
  UNSPEC_UMINP = 481,
  UNSPEC_UMULHRS = 482,
  UNSPEC_UMULHS = 483,
  UNSPEC_UMULLB = 484,
  UNSPEC_UMULLT = 485,
  UNSPEC_UQRSHRNB = 486,
  UNSPEC_UQRSHRNT = 487,
  UNSPEC_UQSHRNB = 488,
  UNSPEC_UQSHRNT = 489,
  UNSPEC_UQXTNB = 490,
  UNSPEC_UQXTNT = 491,
  UNSPEC_USHLLB = 492,
  UNSPEC_USHLLT = 493,
  UNSPEC_USUBLB = 494,
  UNSPEC_USUBLT = 495,
  UNSPEC_USUBWB = 496,
  UNSPEC_USUBWT = 497,
  UNSPEC_USDOT = 498,
  UNSPEC_SUDOT = 499,
  UNSPEC_BFDOT = 500,
  UNSPEC_BFMLALB = 501,
  UNSPEC_BFMLALT = 502,
  UNSPEC_BFMMLA = 503,
  UNSPEC_BFCVTN = 504,
  UNSPEC_BFCVTN2 = 505,
  UNSPEC_BFCVT = 506
};
#define NUM_UNSPEC_VALUES 507
extern const char *const unspec_strings[];

enum unspecv {
  UNSPECV_EH_RETURN = 0,
  UNSPECV_GET_FPCR = 1,
  UNSPECV_SET_FPCR = 2,
  UNSPECV_GET_FPSR = 3,
  UNSPECV_SET_FPSR = 4,
  UNSPECV_BLOCKAGE = 5,
  UNSPECV_PROBE_STACK_RANGE = 6,
  UNSPECV_SPECULATION_BARRIER = 7,
  UNSPECV_BTI_NOARG = 8,
  UNSPECV_BTI_C = 9,
  UNSPECV_BTI_J = 10,
  UNSPECV_BTI_JC = 11,
  UNSPECV_TSTART = 12,
  UNSPECV_TCOMMIT = 13,
  UNSPECV_TCANCEL = 14,
  UNSPEC_RNDR = 15,
  UNSPEC_RNDRRS = 16,
  UNSPECV_LX = 17,
  UNSPECV_SX = 18,
  UNSPECV_LDA = 19,
  UNSPECV_STL = 20,
  UNSPECV_ATOMIC_CMPSW = 21,
  UNSPECV_ATOMIC_EXCHG = 22,
  UNSPECV_ATOMIC_CAS = 23,
  UNSPECV_ATOMIC_SWP = 24,
  UNSPECV_ATOMIC_OP = 25,
  UNSPECV_ATOMIC_LDOP_OR = 26,
  UNSPECV_ATOMIC_LDOP_BIC = 27,
  UNSPECV_ATOMIC_LDOP_XOR = 28,
  UNSPECV_ATOMIC_LDOP_PLUS = 29
};
#define NUM_UNSPECV_VALUES 30
extern const char *const unspecv_strings[];

#endif /* GCC_INSN_CONSTANTS_H */
