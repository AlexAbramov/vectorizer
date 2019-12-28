#pragma once

class BitArray
{
	friend class BitArray;
	UINT bitSize;
  BYTE* byteArray;
public:
	bool FromNBitString(const CString& s,const BYTE nbit);
	CString ToNBitString(const BYTE nbit)const;
	BYTE* ByteArray()const{return byteArray;}
	void EnsureCapacity(UINT bitSize);
	BitArray(const UINT bitSize=0): bitSize(0),byteArray(0)
	{
		BitSize(bitSize);
		Clear();
	}
  BitArray(const BitArray& original): bitSize(0),byteArray(0)
  {
		BitSize(original.BitSize());
    memcpy(byteArray, original.byteArray,ByteSize());
  }
  BitArray(const BYTE* bytes,const int byteSize): bitSize(0),byteArray(0)
  {
		BitSize(byteSize<<3);
    memcpy(byteArray,bytes,ByteSize());
  }
	~BitArray(){if(byteArray) delete[] byteArray;}
	UINT BitSize() const {return bitSize;}
	UINT ByteSize() const {return (bitSize+7)>>3;}
	UINT WordSize() const {return (bitSize+15)>>4;}
	UINT DwordSize() const {return (bitSize+31)>>5;}
	void BitSize(UINT bitSize);
	bool GetBit(UINT i)const{ASSERT(i<bitSize);return byteArray[i>>3]&(1<<(i%8));}
	void SetBit(UINT i,bool v=true){ASSERT(i<bitSize);if(v) byteArray[i>>3]|=1<<(i%8);else byteArray[i>>3]&=~(1<<(i%8));}
	BYTE GetByte(UINT i)const{ASSERT(i<ByteSize());return byteArray[i];}
	void SetByte(UINT i,BYTE b){ASSERT(i<ByteSize());byteArray[i]=b;}
	WORD GetWord(UINT i)const
	{
		ASSERT(i<WordSize());
		if(i==WordSize()-1 && ByteSize()%2==1) return byteArray[ByteSize()-1];
		return ((WORD*)byteArray)[i];
	}
	void SetWord(UINT i,WORD w)
	{
		ASSERT(i<WordSize());
		if(i==WordSize()-1 && ByteSize()%2==1){ASSERT(w<=0xFF);byteArray[ByteSize()-1]=(BYTE)w;}
		else ((WORD*)byteArray)[i]=w;
	}
	DWORD GetDword(UINT i)const;
	void SetDword(UINT i,DWORD dw);
	void Clear(){memset(byteArray, 0, ByteSize());}
	UINT Checksum()const;
	void Checksum(UINT sum);
	WORD WordChecksum()const;
	void WordChecksum(WORD sum);
  BitArray& operator=(const BitArray &original)
  {
    BitSize(original.BitSize());
    memcpy(byteArray,original.byteArray,ByteSize());
    return *this;
  }
  bool operator==(const BitArray &target)
  {
    return bitSize==target.bitSize && !memcmp(byteArray,target.byteArray,ByteSize());
  }
  bool operator!=(const BitArray &target){return !(operator==(target));}
	void FromByte(BYTE b){BitSize(8);byteArray[0]=b;}
	void FromWord(WORD w){BitSize(16);SetWord(0,w);}
	void FromDword(DWORD dw){BitSize(32);((UINT*)byteArray)[0]=dw;}
	bool FromHex(const CString& s){return FromNBitString(s,4);}
	bool From6bitString(const CString& s){return FromNBitString(s,6);}
	CString ToBin()const{return ToNBitString(1);}
	CString ToOct()const{return ToNBitString(3);}
	CString ToHex()const{return ToNBitString(4);}
	CString To6bitString()const{return ToNBitString(6);}
  UINT BitCount() const;
	bool operator[](UINT i)const{return GetBit(i);}
};

class Integer: public BitArray
{
	void AddWord(UINT pos,WORD w);
	void Multiply(WORD w);
public:
	UINT BitSize1()const {UINT i=BitSize(); while(i) if(GetBit(--i)) return i+1; return 0;}
	UINT ByteSize1()const {UINT i=ByteSize(); while(i) if(GetByte(--i)) return i+1;	return 0;}
	UINT WordSize1()const {UINT i=WordSize(); while(i) if(GetWord(--i)) return i+1;	return 0;}
	Integer(): BitArray(){}
	Integer& operator *=(WORD w){Multiply(w);return *this;}
	Integer operator * (const Integer& multiplier)const;
	bool Divide(const WORD divider,WORD& remainder);
	bool FromNRadixString(const CString& s,const BYTE nradix);
	bool FromDec(const CString& s){return FromNRadixString(s,10);}
	CString ToDec()const{return ToNRadixString(10);}
	bool operator == (const Integer& i){return ByteSize1()==i.ByteSize1() && !memcmp(ByteArray(),i.ByteArray(),ByteSize1());}
	CString ToNRadixString(const BYTE nradix)const;
};