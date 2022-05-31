#pragma once

/*==========================================================================*/
/*==========================================================================*/
/*!
	@file		UtilMacro.h
	@brief		マクロクラス
	@author		shiba
	@date		2012/3/1 create this file.

*/
/*==========================================================================*/
/*==========================================================================*/

//---------------------------------------------------------------------------
/*!
	@brief	NEWマクロ
	@note	これから色々機能つけます。Initを確実に呼び出します。
*/
//---------------------------------------------------------------------------
/*
template < typename T, typename C >
inline void CLASS_NEW(T*& p, C type )
{
	p = new type;
	p->Init();
}
*/

#define	CLASS_NEW( class, type )	\
	class = new type;				\
	class->Init();					\

//---------------------------------------------------------------------------
/*!
	@brief	DESTROYマクロ
	@note	これから色々機能つけます。Destroyを確実に呼び出します。
*/
//---------------------------------------------------------------------------
template <class T>
inline void CLASS_DESTROY(T*& p)
{
	if (p)
	{
		p->Destroy();
		SAFE_DELETE(p);
	}
}

//---------------------------------------------------------------------------
/*!
	@brief	COM開放
*/
//---------------------------------------------------------------------------
template <class T>
inline void SAFE_RELEASE(T*& p)
{
	if ( p ) 
	{
		p->Release(); 
		p = nullptr;
	}
}

//---------------------------------------------------------------------------
/*!
	@brief	通常消去
*/
//---------------------------------------------------------------------------
template <class T>
inline void SAFE_DELETE( T*& p )
{
	if (p)
	{
		delete p; 
		p = nullptr;
	}
}

template <class T>
inline void SAFE_DELETE_TASK(T*& p)
{
	if (p)
	{
		p->DelTask();
		p = nullptr;
	}
}

//---------------------------------------------------------------------------
/*!
	@brief	配列消去
*/
//---------------------------------------------------------------------------
template <class T>
inline void SAFE_DELETEA(T*& p)
{
	if (p)
	{ 
		delete[] p;
		p = nullptr;
	}
}

//---------------------------------------------------------------------------
/*!
	@brief	アサート
*/
//---------------------------------------------------------------------------
#define NS_ASSERT( _x ) assert( _x )

//---------------------------------------------------------------------------
/*!
	@brief	未使用宣言マクロ（警告消去）
*/
//---------------------------------------------------------------------------
#define NS_UNUSED( _x ) ( void )( _x )

//---------------------------------------------------------------------------
/*!
	@brief	配列サイズ
*/
//---------------------------------------------------------------------------
#if true
#define SIZE_OF_ARRAY(ary)  (sizeof(ary)/sizeof((ary)[0]))
#else
template
<
	typename TYPE,
	std::size_t SIZE
>
std::size_t SIZE_OF_ARRAY(const TYPE(&)[SIZE])
{
	return SIZE;
}
#endif

////---------------------------------------------------------------------------
///*!
//	@brief	キャスト
//*/
////---------------------------------------------------------------------------
#define NS_STATIC_CAST( _type, _data )		static_cast< _type >( _data )
#define NS_REINTERPRET_CAST( _type, _data )	reinterpret_cast< _type >( _data )

//--------------------------------------------------------------------
// シングルトンマクロ。
// NS_SINGLETONをクラスのプライベート部分に付け加えて
// HかCPPにNS_SINGLETON_INSTANCE_DEFINEで宣言する。
//--------------------------------------------------------------------
//---------------------------------------------------------------------------
/*!
	@brief	シングルトンマクロ
*/
//---------------------------------------------------------------------------
#define	NS_SINGLETON( _class )										\
	private:														\
		void operator =(const _class& src);							\
		_class(const _class& src);									\
	public:															\
		static	_class*	CreateInstance( void )						\
		{															\
			if( m_pcsInstance == nullptr )								\
			{														\
				m_pcsInstance = new _class();						\
				if( !m_pcsInstance->Init() )						\
				{													\
					DestroyInstance();								\
					return nullptr;									\
				}													\
			}														\
			return m_pcsInstance;									\
		}															\
		static	void	DestroyInstance( void )						\
		{															\
			if( m_pcsInstance )										\
			{														\
				delete m_pcsInstance;								\
				m_pcsInstance = nullptr;								\
			}														\
		}															\
		static	inline	_class*	GetInstance( void )					\
		{															\
			return m_pcsInstance;									\
		}															\
	private:														\
		static	_class*		m_pcsInstance;							\

//----------------------------------------------------------------------------
/*!
	@brief	シングルトン実体宣言
*/
//----------------------------------------------------------------------------
#define	NS_SINGLETON_INSTANCE_DEFINE( _class )							\
	_class*	_class::m_pcsInstance	= nullptr;								\


//---------------------------------------------------------------------------
/*!
	@brief	enum演算用オペレーター
*/
//---------------------------------------------------------------------------
#define FUNC_ENUM_OPERATOR( _enum, _min, _max )			\
	inline	_enum&	operator ++ ( _enum& reInc )		\
	{													\
		reInc = (_enum)(reInc + 1);						\
		AZ_ASSERT( reInc <= (_max) );					\
		return reInc;									\
	}													\
	inline	_enum&	operator -- ( _enum& reDec )		\
	{													\
		reDec = (_enum)( reDec - 1 );					\
		AZ_ASSERT( reDec >= (_min) );					\
		return reDec;									\
	}													\
	inline	_enum	operator ++ ( _enum& reInc, int )	\
	{													\
		_enum	eTmp( reInc );							\
		reInc	= (_enum)( reInc + 1 );					\
		AZ_ASSERT( reInc <= (_max) );					\
		return eTmp;									\
	}													\
	inline	_enum	operator -- ( _enum& reDec, int )	\
	{													\
		_enum	eTmp( reDec );							\
		reDec	= (_enum)( reDec - 1 );					\
		AZ_ASSERT( reDec >= (_min) );					\
		return eTmp;									\
	}

//---------------------------------------------------------------------------
/*!
	@brief	参照カウンタクラス
*/
//---------------------------------------------------------------------------
template <class T>
class CRefCountSys
{
public:
	CRefCountSys()
	:
	pResource( nullptr ),
	unRefCount( 0 ){}
	~CRefCountSys()
	{
		SAFE_DELETE( pResource );
	}

public:
	//!< リソース
	T* pResource;

	//!< 参照カウンタ
	u32 unRefCount;
};

//EOF