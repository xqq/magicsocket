#pragma once

namespace Magic {
	class NoRebindTag {
	};

	template <typename T>
	class Pointer {
		template <typename U>
		friend class Pointer;
	public:
		Pointer(T *t = new T);
		Pointer(const Pointer <T> &t);

		template <typename U>
		Pointer(const U &u, NoRebindTag)
		{
			Bind(new T(u));
		}

		template <typename U, typename V>
		Pointer(const U &u, const V &v)
		{
			Bind(new T(u, v));
		}

		template <typename U, typename V, typename W>
		Pointer(const U &u, const V &v, const W &w)
		{
			Bind(new T(u, v, w));
		}

		template <typename U, typename V, typename W, typename X>
		Pointer(const U &u, const V &v, const W &w, const X &x)
		{
			Bind(new T(u, v, w, x));
		}

		template <typename U, typename V, typename W, typename X, typename Y>
		Pointer(const U &u, const V &v, const W &w, const X &x, const Y &y)
		{
			Bind(new T(u, v, w, x, y));
		}

		template <typename U, typename V, typename W, typename X, typename Y, typename Z>
		Pointer(const U &u, const V &v, const W &w, const X &x, const Y &y, const Z &z)
		{
			Bind(new T(u, v, w, x, y, z));
		}

		~Pointer();
		Pointer <T> & operator =(T *t);
		Pointer <T> & operator =(const Pointer <T> &t);

		T & operator *() const;
		T * operator ->() const;

		bool operator ==(const Pointer <T> &t) const;
		bool operator !=(const Pointer <T> &t) const;

		// rebind
		template <typename U>
		Pointer(const Pointer <U> &u)
		{
			Bind((T *)u.pointer);
		}

		template <typename U>
		Pointer <T> & operator =(const Pointer <U> &u)
		{
			if (pointer != (T *)u.pointer) {
				Unbind();
				Bind((T *)u.pointer);
			}
			return *this;
		}

		template <typename U>
		bool operator ==(const Pointer <U> &u) const
		{
			return pointer == (T *)u.pointer;
		}

		template <typename U>
		bool operator !=(const Pointer <U> &u) const
		{
			return pointer != (T *)u.pointer;
		}

	private:
		void Bind(T *p);
		void Unbind();

		T *pointer;
	};

	template <typename T>
	void Pointer <T>::Bind(T *p)
	{
		p->AddRef();
		pointer = p;
	}

	template <typename T>
	void Pointer <T>::Unbind()
	{
		pointer->Release();
	}

	template <typename T>
	Pointer <T>::Pointer(T *t)
	{
		Bind(t);
	}

	template <typename T>
	Pointer <T>::Pointer(const Pointer <T> &t)
	{
		Bind(t.pointer);
	}

	template <typename T>
	Pointer <T>::~Pointer()
	{
		Unbind();
	}

	template <typename T>
	Pointer <T> & Pointer <T>::operator =(T *t)
	{
		Unbind();
		Bind(t);
		return *this;
	}

	template <typename T>
	Pointer <T> & Pointer <T>::operator =(const Pointer <T> &t)
	{
		Unbind();
		Bind(t.pointer);
		return *this;
	}

	template <typename T>
	T & Pointer <T>::operator *() const
	{
		return *pointer;
	}

	template <typename T>
	T * Pointer <T>::operator ->() const
	{
		return pointer;
	}

	template <typename T>
	bool Pointer <T>::operator ==(const Pointer <T> &t) const
	{
		return pointer == t.pointer;
	}

	template <typename T>
	bool Pointer <T>::operator !=(const Pointer <T> &t) const
	{
		return pointer != t.pointer;
	}
}