#ifndef tlf_h
#define tlf_h
/*******************************************************************/
#include "tree.h"
/********************************************************************/
#define TLFisLTL(fml)         (((TLFnnftype(fml)&0x03)==0))
#define TLFisECTLx(fml)       (((TLFnnftype(fml)&0x01)==0)&&TLFisCTLx(fml))
#define TLFisACTLx(fml)       (((TLFnnftype(fml)&0x02)==0)&&TLFisCTLx(fml))
/********************************************************************/
#include "wftlf.h"
#endif
