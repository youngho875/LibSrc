// GDatumTrans.h

#pragma once



class CGPointGeo;
class CGRectGeo;
class CGRectGP;

// WGS 84 Å¸¿øÃ¼ÀÇ WGS 84 µ¥ÀÌÅÒ ÁÂÇ¥¸¦ Bessel 1841ÀÇ Tokyo datumÁÂÇ¥·Î º¯È¯ÇÑ´Ù.
extern __declspec( dllexport )
void DTWgsToTok(CGPointGeo *pToGPs, CGPointGeo *pFromGPs, int nCount);
//extern __declspec( dllexport )
//void DTWgsToTok(CGRectGP *pToRectGP, CGRectGP *pFromRectGP);

// Bessel 1841ÀÇ Tokyo datumÁÂÇ¥¸¦ WGS 84 Å¸¿øÃ¼ÀÇ WGS 84 µ¥ÀÌÅÒ ÁÂÇ¥·Î º¯È¯ÇÑ´Ù.
extern __declspec( dllexport )
void DTTokToWgs(CGPointGeo *pToGPs, CGPointGeo *pFromGPs, int nCount);
//extern __declspec( dllexport )
//void DTTokToWgs(CGRectGP *pToRectGP, CGRectGP *pFromRectGP)

