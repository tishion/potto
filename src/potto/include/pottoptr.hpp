#ifndef POTTOPTR_H_
#define POTTOPTR_H_
#pragma once

#include <assert.h>

/// <summary>
/// The potto namespace.
/// </summary>
namespace Potto
{
	/// <summary>
	/// The smart pointer of potto object.
	/// </summary>
	template<class T>
	class PottoPtr
	{
	private:
		/// <summary>
		/// 
		/// </summary>
		typedef PottoPtr this_type;

	public:
		/// <summary>
		/// 
		/// </summary>
		typedef T element_type;

		/// <summary>
		/// 
		/// </summary>
		PottoPtr() : px(0)
		{
		}

		PottoPtr(T* p, bool add_ref = true) : px(p)
		{
			if (px != 0 && add_ref) px->AddRef();
		}

#if !defined(NO_MEMBER_TEMPLATES) || defined(MSVC6_MEMBER_TEMPLATES)

		/// <summary>
		/// 
		/// </summary>
		template<class U>
		PottoPtr(PottoPtr<U> const& rhs) : px(rhs.get())
		{
			if (px != 0) px->AddRef();
		}

#endif

		/// <summary>
		/// 
		/// </summary>
		PottoPtr(PottoPtr const & rhs) : px(rhs.px)
		{
			if (px != 0) px->AddRef();
		}

		/// <summary>
		/// 
		/// </summary>
		~PottoPtr()
		{
			if (px != 0) px->Release();
		}

#if !defined(NO_MEMBER_TEMPLATES) || defined(MSVC6_MEMBER_TEMPLATES)

		/// <summary>
		/// 
		/// </summary>
		template<class U> 
		PottoPtr& operator=(PottoPtr<U> const& rhs)
		{
			this_type(rhs).swap(*this);
			return *this;
		}

#endif

		// Move support

#if !defined( NO_CXX11_RVALUE_REFERENCES )

		/// <summary>
		/// 
		/// </summary>
		PottoPtr(PottoPtr&& rhs) : px(rhs.px)
		{
			rhs.px = 0;
		}

		PottoPtr& operator=(PottoPtr&& rhs)
		{
			this_type(static_cast<PottoPtr &&>(rhs)).swap(*this);
			return *this;
		}

#endif

		/// <summary>
		/// 
		/// </summary>
		PottoPtr& operator=(PottoPtr const & rhs)
		{
			this_type(rhs).swap(*this);
			return *this;
		}

		/// <summary>
		/// 
		/// </summary>
		PottoPtr& operator=(T* rhs)
		{
			this_type(rhs).swap(*this);
			return *this;
		}

		void reset()
		{
			this_type().swap(*this);
		}

		void reset(T* rhs)
		{
			this_type(rhs).swap(*this);
		}

		void reset(T* rhs, bool add_ref)
		{
			this_type(rhs, add_ref).swap(*this);
		}

		T* get() const
		{
			return px;
		}

		T* detach()
		{
			T* ret = px;
			px = 0;
			return ret;
		}

		T& operator*() const
		{
			assert(px != 0);
			return *px;
		}

		T** operator&()
		{
			assert(px == nullptr);
			return &px;
		}

		T* operator->() const
		{
			assert(px != 0);
			return px;
		}

		operator bool() const
		{
			return px != 0;
		}

		bool operator! () const
		{
			return px == 0;
		}

		void swap(PottoPtr& rhs)
		{
			T* tmp = px;
			px = rhs.px;
			rhs.px = tmp;
		}

	private:

		T* px;
	};

	template<class T, class U> 
	inline bool operator==(PottoPtr<T> const & a, PottoPtr<U> const & b)
	{
		return a.get() == b.get();
	}

	template<class T, class U> 
	inline bool operator!=(PottoPtr<T> const & a, PottoPtr<U> const & b)
	{
		return a.get() != b.get();
	}

	template<class T, class U> 
	inline bool operator==(PottoPtr<T> const & a, U * b)
	{
		return a.get() == b;
	}

	template<class T, class U> 
	inline bool operator!=(PottoPtr<T> const & a, U * b)
	{
		return a.get() != b;
	}

	template<class T, class U> 
	inline bool operator==(T * a, PottoPtr<U> const & b)
	{
		return a == b.get();
	}

	template<class T, class U> 
	inline bool operator!=(T * a, PottoPtr<U> const & b)
	{
		return a != b.get();
	}

#if __GNUC__ == 2 && __GNUC_MINOR__ <= 96

	// Resolve the ambiguity between our op!= and the one in rel_ops

	template<class T> 
	inline bool operator!=(PottoPtr<T> const & a, PottoPtr<T> const & b)
	{
		return a.get() != b.get();
	}

#endif

#if !defined( NO_CXX11_NULLPTR )

	template<class T> 
	inline bool operator==(PottoPtr<T> const & p, std::nullptr_t)
	{
		return p.get() == 0;
	}

	template<class T> 
	inline bool operator==(std::nullptr_t, PottoPtr<T> const & p)
	{
		return p.get() == 0;
	}

	template<class T> 
	inline bool operator!=(PottoPtr<T> const & p, std::nullptr_t)
	{
		return p.get() != 0;
	}

	template<class T> 
	inline bool operator!=(std::nullptr_t, PottoPtr<T> const & p)
	{
		return p.get() != 0;
	}

#endif

	template<class T> 
	inline bool operator<(PottoPtr<T> const & a, PottoPtr<T> const & b)
	{
		return std::less<T*>()(a.get(), b.get());
	}

	template<class T> 
	void swap(PottoPtr<T>& lhs, PottoPtr<T>& rhs)
	{
		lhs.swap(rhs);
	}

	// mem_fn support

	template<class T> 
	T* get_pointer(PottoPtr<T> const & p)
	{
		return p.get();
	}

	template<class T, class U> 
	PottoPtr<T> static_pointer_cast(PottoPtr<U> const & p)
	{
		return static_cast<T*>(p.get());
	}

	template<class T, class U> 
	PottoPtr<T> const_pointer_cast(PottoPtr<U> const & p)
	{
		return const_cast<T*>(p.get());
	}

	template<class T, class U> 
	PottoPtr<T> dynamic_pointer_cast(PottoPtr<U> const & p)
	{
		return dynamic_cast<T*>(p.get());
	}
}

#endif // POTTOPTR_H_