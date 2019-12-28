#include "StdAfx.h"
#include "Math.h"
#include "BitArray.h"

// BIT ARRAY
//////////////

static const BYTE conversionFailed=0xFF;

static BYTE charToInt(TCHAR c)
{
	if(c>=_T('0')&&c<=_T('9')) return c-_T('0');
	if(c>=_T('A')&&c<=_T('Z')) return 10+c-_T('A');
	if(c>=_T('a')&&c<=_T('z')) return 36+c-_T('a');
	if(c==_T('^')) return 62;
	if(c==_T('&')) return 63;
	return conversionFailed;
}

static BYTE charToIntCaseIns(TCHAR c)
{
	if(c>=_T('0')&&c<=_T('9')) return c-_T('0');
	if(c>=_T('A')&&c<=_T('Z')) return 10+c-_T('A');
	if(c>=_T('a')&&c<=_T('z')) return 10+c-_T('a');
	return conversionFailed;
}

static TCHAR intToChar(BYTE i)
{
	if(i>=0&&i<=9) return i+_T('0');
	if(i>=10&&i<=35) return i-10+_T('A');
	if(i>=36&&i<=61) return i-36+_T('a');
	if(i==62) return _T('^');
	if(i==63) return _T('&');
	return conversionFailed;
}

bool BitArray::FromNBitString(const CString& s,const BYTE nbit)
{
	ASSERT(nbit>=1&&nbit<=6);
	int strLength=s.GetLength();
	BitSize(strLength*nbit);
	Clear();
	for(int i=0;i<strLength;i++)
	{
		TCHAR c=s[strLength-1-i];
		TCHAR sum=charToInt(c);
		if(sum==conversionFailed) return false;
		if(sum>=1<<nbit) return false;
		if(sum==0) continue;
		for(int j=0;j<nbit;j++)
		{
			bool b=sum&(1<<j);
			if(b) SetBit(i*nbit+j,b);
		}
	}
	return true;
}

CString BitArray::ToNBitString(const BYTE nbit)const
{
	ASSERT(nbit>=1&&nbit<=6);
	int slength=(bitSize+nbit-1)/nbit;
	CString s((TCHAR)0,slength);
	TCHAR sum=0;
	for(int i=0;i<bitSize;i++)
	{
		if(GetBit(i)) sum+=1<<(i%nbit);
		if(i%nbit==nbit-1||i==bitSize-1)
		{
			ASSERT(sum>=0&&sum<(1<<nbit));
			TCHAR c=intToChar(sum);
			ASSERT(c!=conversionFailed);
			s.SetAt(slength-i/nbit-1,c);
			sum=0;
		}
	}
	return s;
}

DWORD BitArray::GetDword(UINT i)const
{
	ASSERT(i<DwordSize());
	if(i==DwordSize()-1)
	{
		UINT byteSize=ByteSize();
		UINT remainder=byteSize%4;
		if(remainder)
		{
			DWORD sum=0;
			for(int j=1;j<=remainder;j++)
			{
			  sum+=byteArray[byteSize-j];
				if(j!=remainder) sum<<=8;
			}
			return sum;
		}
	}
	return ((DWORD*)byteArray)[i];
}

void BitArray::BitSize(UINT bitSize)
{
	UINT byteSize0=ByteSize();
	this->bitSize=bitSize;
	if(byteSize0==ByteSize()) return;
	if(byteArray) delete [] byteArray;
	byteArray=new BYTE[ByteSize()];
}

void BitArray::SetDword(UINT i,DWORD dw)
{
	ASSERT(i<DwordSize());
	if(i==DwordSize()-1)
	{
		UINT byteSize=ByteSize();
		UINT remainder=byteSize%4;
		if(remainder)
		{
			BYTE* ptr=(BYTE*)&dw;
			for(UINT j=0;j<remainder;j++) byteArray[byteSize-remainder+j]=ptr[j];
			ASSERT((remainder==1&&dw<=0xFF)||(remainder==2&&dw<=0xFFFF)||(remainder==3&&dw<=0xFFFFFF));
			return;
		}
	}
	((DWORD*)byteArray)[i]=dw;
}

UINT BitArray::Checksum()const
{
	UINT sum=0;
	UINT i=DwordSize();
	while(i) sum+=GetDword(--i);
	return sum;
}

void BitArray::Checksum(UINT sum)
{
	UINT dif=sum-Checksum();
	if(!dif) return;
	SetDword(0,GetDword(0)+dif);
}

WORD BitArray::WordChecksum()const
{
	WORD sum=0;
	UINT i=WordSize();
	while(i) sum+=GetWord(--i);
	return sum;
}

void BitArray::WordChecksum(WORD sum)
{
	WORD dif=sum-WordChecksum();
	if(!dif) return;
	SetWord(0,GetWord(0)+dif);
}

void BitArray::EnsureCapacity(UINT bitSize)
{
	if((bitSize+7)>>3 > ByteSize()) BitSize(bitSize);
}

UINT BitArray::BitCount() const
{
	UINT bitCount=0;
	for(UINT i=0;i<bitSize;i++)	if(GetBit(i)) bitCount++;
	return bitCount;
}

// INTEGER
////////////

bool Integer::FromNRadixString(const CString& s,const BYTE radix)
{
	ASSERT(radix>=2&&radix<=64);
	UINT sz=(UINT)(1.001+s.GetLength()*log((double)radix)/log(2.));
	EnsureCapacity(sz);
	Clear();
	for(int i=0;i<s.GetLength();i++)
	{
		if(i) Multiply(radix);
		BYTE b=charToInt(s[i]);
//		ASSERT(b<radix);
		if(b==conversionFailed) return false;
		if(b) AddWord(0,b);
	}
	return true;
}

CString Integer::ToNRadixString(const BYTE radix)const
{
	CString res;
	ASSERT(radix>=2&&radix<=64);
	Integer i(*this);
	WORD remainder;
	bool cont=true;
	while(cont)
	{
		cont=i.Divide(radix,remainder);
		ASSERT(remainder<radix);
		res=intToChar(remainder)+res;
	}
	return res;
}

bool Integer::Divide(const WORD divider, WORD &remainder)
{
	ASSERT(divider!=0);
	UINT wordSize1=WordSize1();
	DWORD dw=0;
	if(wordSize1<=2)
	{
		dw=GetDword(0);
		if(dw<divider)
		{
			remainder=(WORD)dw;
			return false;
		}
		remainder=dw%divider;
		SetDword(0,dw/divider);
		return true;
	}
	UINT i=wordSize1;
	while(i)
	{
		if(dw) dw<<=16;
		dw+=GetWord(--i);
		if(dw>=divider)
		{
			ASSERT(dw/divider<=0xFFFF);
			SetWord(i,dw/divider);
			dw=dw%divider;
		}
		else
		{
			SetWord(i,0);
		}
	}
	ASSERT(dw<=0xFFFF);
	remainder=dw;
	ASSERT(remainder<divider);
	return true;
}

Integer Integer::operator *(const Integer &multiplier)const
{
	Integer res;
	res.BitSize(BitSize1()+multiplier.BitSize1());
	res.Clear();
	UINT i=WordSize1();
	while(i)
	{
		WORD w1=GetWord(--i);
		if(!w1) continue;
		UINT j=multiplier.WordSize1();
		while(j)
		{
			WORD w2=multiplier.GetWord(--j);
			if(!w2) continue;
			UINT m=w1;
			m*=w2;
			res.AddWord(i+j,(WORD)m);
			WORD w=(WORD)(m>>16);
			if(w) res.AddWord(i+j+1,w);
		}
	}
	return res;
}

void Integer::AddWord(UINT pos, WORD w)
{
	UINT sum=w;
	while(sum)
	{
		ASSERT(pos<WordSize());
		sum+=GetWord(pos);
		SetWord(pos,(WORD)sum);
		sum>>=16;
		pos++;
	}
}

void Integer::Multiply(WORD w)
{
	UINT i=WordSize1();
	while(i)
	{
		UINT m=GetWord(--i);
		if(!m) continue;
		m*=w;
		SetWord(i,(WORD)m);
		WORD w1=(WORD)(m>>16);
		if(w1) AddWord(i+1,w1);
	}
}