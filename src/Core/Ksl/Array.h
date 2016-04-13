/*
 * Copyright (C) 2016  Elvis Teixeira
 *
 * This source code is free software: you can redistribute it
 * and/or modify it under the terms of the GNU Lesser General
 * Public License as published by the Free Software Foundation,
 * either version 3 of the License, or (at your option) any
 * later version.
 *
 * This source code is distributed in the hope that it will be
 * useful, but WITHOUT ANY WARRANTY; without even the implied
 * warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 * PURPOSE.  See the GNU Lesser General Public License for
 * more details.
 *
 * You should have received a copy of the GNU Lesser General
 * Public License along with this program.
 * If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef KSL_ARRAY_H
#define KSL_ARRAY_H

#include <Ksl/Global.h>
#include <Ksl/Math.h>
#include <vector>
#include <list>
#include <ostream>
#include <QVector>
#include <QList>
#include <QTextStream>
#include <QDebug>

namespace Ksl {

/*****************************************************
* All array types in Ksl are specializations
* this class template
****************************************************/
template <int D, typename T=double> class Array{};


/***************************************************
* Storage engine used by public array types
**************************************************/
template <typename T>
class Array<0,T>
{
private:
   
   friend class Array<1,T>;
   friend class Array<2,T>;
   
   enum Type {
      ROW_VECTOR     = 0x00000001,
      COL_VECTOR     = 0x00000002,
      MATRIX         = 0x00000004,
      IS_VIEW        = 0x00000008
   };
   
   
   inline Array(int rows, int cols, int type);
   
   inline int size() const { return m_rows*m_cols; }
   
   inline T& vec_at(int idx);
   inline const T& vec_at(int idx) const;
   
   inline T* mat_at(int idx);
   inline const T* mat_at(int idx) const;
   
   
   inline void resize(int rows, int cols);
   inline void reserve(int nalloc);
   inline void append(const T &value);
   inline void free();
   inline Array* ref();
   inline bool unref();
   
   
   int m_type;
   int m_rows;
   int m_cols;
   int m_nalloc;
   int m_refs;
   T *m_data;
};


template <typename T> inline
Array<0,T>::Array(int rows, int cols, int type)
{
   m_type = type;
   m_refs = 1;
   m_rows = 0;
   m_cols = 0;
   m_data = nullptr;
   resize(rows, cols);
}


template <typename T> inline
T& Array<0,T>::vec_at(int idx)
{
   return m_data[idx];
}


template <typename T> inline
const T& Array<0,T>::vec_at(int idx) const
{
   return m_data[idx];
}


template <typename T> inline
T* Array<0,T>::mat_at(int idx)
{
   return m_data + idx*m_cols;
}


template <typename T> inline
const T* Array<0,T>::mat_at(int idx) const
{
   return m_data + idx*m_cols;
}


template <typename T> inline
void Array<0,T>::resize(int rows, int cols)
{
   if (m_type & IS_VIEW) {
      return;
   }
   if (rows != m_rows || cols != m_cols) {
      m_rows = rows;
      m_cols = cols;
      m_nalloc = rows*cols;
      if (m_nalloc > 0) {
         if (m_data == nullptr) {
            m_data = (T*) std::malloc((std::size_t) m_nalloc*sizeof(T));
         } else {
            m_data = (T*) std::realloc(
               (void*) m_data,
               (std::size_t) m_nalloc*sizeof(T));
         }
      } else {
         free();
      }
   }
}


template <typename T> inline
void Array<0,T>::reserve(int nalloc)
{
   if (m_type & IS_VIEW) {
      return;
   }
   if (nalloc > 0 && nalloc > m_nalloc) {
      m_nalloc = nalloc;
      if (m_data == nullptr) {
         m_data = (T*) std::malloc((std::size_t) m_nalloc*sizeof(T));
      } else {
         m_data = (T*) std::realloc(
            (void*) m_data,
            (std::size_t) m_nalloc*sizeof(T));
      }
   }
}


template <typename T> inline
void Array<0,T>::append(const T &value)
{
   if (m_type & IS_VIEW || m_type & COL_VECTOR) {
      return;
   }
   if (m_nalloc < 12) {
      reserve(12);
   } else if (m_nalloc == m_cols) {
      reserve(4*m_cols/3);
   }
   m_data[m_cols] = value;
   m_cols += 1;
}


template <typename T>
inline void Array<0,T>::free()
{
   if (m_type & IS_VIEW) {
      return;
   }
   if (m_data != nullptr) {
      std::free(m_data);
      m_data = nullptr;
   }
}


template <typename T>
inline Array<0,T>* Array<0,T>::ref()
{
   m_refs += 1;
   return this;
}


template <typename T>
inline bool Array<0,T>::unref()
{
   m_refs -= 1;
   if (m_refs == 0) {
      free();
      return true;
   }
   return false;
}



/******************************************************
* Array 1D (vector)
******************************************************/


template <typename T>
class Array<1,T>
{
public:
   
   
   inline Array(int size=0);
   inline Array(int size, const T &value);
   inline Array(const Array &that);
   inline Array(Array &&that);
   
   inline ~Array();
   
   inline Array& operator= (const Array &that);
   inline Array& operator= (Array &&that);
   
   
   inline int size() const { return m_data!=nullptr ? m_data->size() : 0; }
   
   inline T& operator[] (int idx) { return m_data->vec_at(idx); }
   inline const T& operator[] (int idx) const { return m_data->vec_at(idx); }
   
   inline T& at(int idx) { return m_data->vec_at(idx); }
   inline const T& at(int idx) const { return m_data->vec_at(idx); }
   
   inline T* begin() { return m_data!=nullptr ? m_data->m_data : nullptr; }
   inline const T* begin() const { return m_data!=nullptr ? m_data->m_data : nullptr; }
   
   inline T* end() { return m_data!=nullptr ? m_data->m_data+m_data->size() : nullptr; }
   inline const T* end() const { return m_data!=nullptr ? m_data->m_data+m_data->size() : nullptr; }
   
   inline T* c_ptr() { return m_data!=nullptr ? m_data->m_data : nullptr; }
   inline const T* c_ptr() const { return m_data!=nullptr ? m_data->m_data : nullptr; }
   
   
   inline void append(const T &value);
   
   
private:
   
   Array<0,T> *m_data;
};


template <typename T> inline
Array<1,T>::Array(int size)
{
   if (size > 0) {
      m_data = new Array<0,T>(1, size, Array<0,T>::ROW_VECTOR);
   } else {
      m_data = nullptr;
   }
}


template <typename T> inline
Array<1,T>::Array(int size, const T &value)
{
   if (size > 0) {
      m_data = new Array<0,T>(1, size, Array<0,T>::ROW_VECTOR);
      for (int k=0; k<size; ++k) {
         at(k) = value;
      }
   } else {
      m_data = nullptr;
   }
}


template <typename T> inline
Array<1,T>::Array(const Array<1,T> &that)
{
   if (that.m_data != nullptr) {
      m_data = that.m_data->ref();
   } else {
      m_data = nullptr;
   }
}


template <typename T> inline
Array<1,T>::Array(Array<1,T> &&that)
{
   if (that.m_data != nullptr) {
      m_data = that.m_data->ref();
   } else {
      m_data = nullptr;
   }
}


template <typename T>
inline Array<1,T>::~Array()
{
   if (m_data != nullptr) {
      if (m_data->unref()) {
         delete m_data;
      }
   }
}


template <typename T> inline
Array<1,T>& Array<1,T>::operator= (const Array<1,T> &that)
{
   if (m_data != that.m_data) {
      if (m_data != nullptr) {
         if (m_data->unref()) {
            delete m_data;
         }
      }
      if (that.m_data != nullptr) {
         m_data = that.m_data->ref();
      } else {
         m_data = nullptr;
      }
   }
   return *this;
}


template <typename T> inline
Array<1,T>& Array<1,T>::operator= (Array<1,T> &&that)
{
   if (m_data != that.m_data) {
      if (m_data != nullptr) {
         if (m_data->unref()) {
            delete m_data;
         }
      }
      if (that.m_data != nullptr) {
         m_data = that.m_data->ref();
      } else {
         m_data = nullptr;
      }
   }
   return *this;
}


template <typename T> inline std::ostream&
operator<< (std::ostream &out, const Array<1,T> &v)
{
   int n = v.size() - 1;
   out << '[';
   for (int k=0; k<n; ++k) {
      out << v[k] << ", ";
   }
   if (n >= 0) out << v[n];
   out << ']';
   return out;
}


template <typename T> inline QDebug
operator<< (QDebug out, const Array<1,T> &v)
{
   int n = v.size() - 1;
   out << '[';
   for (int k=0; k<n; ++k) {
      out << v[k] << ", ";
   }
   if (n >= 0) out << v[n];
   out << ']';
   return out;
}


template <typename T> inline
void Array<1,T>::append(const T &value)
{
   if (m_data == nullptr) {
      m_data = new Array<0,T>(1, 1, Array<0,T>::ROW_VECTOR);
      at(0) = value;
   } else {
      m_data->append(value);
   }
}


template <typename T=double> inline
Array<1,T> zeros(int size)
{
   Array<1,T> ret(size, T(0));
   return std::move(ret);
}


template <typename T=double> inline
Array<1,T> ones(int size)
{
   Array<1,T> ret(size, T(1));
   return std::move(ret);
}


template <typename T=double> inline
Array<1,T> linspace(const T &min, const T &max, const T &step=T(1))
{
   Array<1,T> ret(int((max-min)/step) + 1);
   for (int k=0; k<ret.size(); ++k) {
      ret[k] = k*step;
   }
   return std::move(ret);
}


template <typename T=double> inline
Array<1,T> randspace(int size, const T &factor=T(1))
{
   Array<1,T> ret(size);
   for (int k=0; k<ret.size(); ++k) {
      ret[k] = T(factor * double(rand())/RAND_MAX);
   }
   return std::move(ret);
}


template <typename T> inline
Array<1,T> samesize(const Array<1,T> &v)
{
   Array<1,T> ret(v.size());
   return std::move(ret);
}


/******************************************************
* Array 2D (matrix)
******************************************************/


template <typename T>
class Array<2,T>
{
public:
   
   inline Array(int rows=0, int cols=0);
   inline Array(int rows, int cols, const T &value);
   inline Array(const Array<2,T> &that);
   inline Array(Array<2,T> &&that);
   
   inline ~Array();
   
   inline Array& operator= (const Array<2,T> &that);
   inline Array& operator= (Array<2,T> &&that);
   
   
   inline int size() const { return m_data!=nullptr ? m_data->size() : 0; }
   inline int rows() const { return m_data!=nullptr ? m_data->m_rows : 0; }
   inline int cols() const { return m_data!=nullptr ? m_data->m_cols : 0; }
   
   inline T* operator[] (int idx) { return m_data->mat_at(idx); }
   inline const T* operator[] (int idx) const { return m_data->mat_at(idx); }
   
   inline T& at(int idx) { return m_data->vec_at(idx); }
   inline const T& at(int idx) const { return m_data->vec_at(idx); }
   
   inline T* begin() { return m_data!=nullptr ? m_data->m_data : nullptr; }
   inline const T* begin() const { return m_data!=nullptr ? m_data->m_data : nullptr; }
   
   inline T* end() { return m_data!=nullptr ? m_data->m_data+m_data->size() : nullptr; }
   inline const T* end() const { return m_data!=nullptr ? m_data->m_data+m_data->size() : nullptr; }
   
   inline T* c_ptr() { return m_data!=nullptr ? m_data->m_data : nullptr; }
   inline const T* c_ptr() const { return m_data!=nullptr ? m_data->m_data : nullptr; }

   inline void append(const T &value);

   inline void reshape(int rows, int cols);

   inline void set_col(int k, const T &value);
   inline void set_col(int k, const Array<1,T> &vec);
   inline void set_row(int k, const T &value);
   inline void set_row(int k, const Array<1,T> &vec);
   
   inline void row_to_row(int k, const Array<2,T> &mat, int j);
   inline void col_to_row(int k, const Array<2,T> &mat, int j);
   inline void row_to_col(int k, const Array<2,T> &mat, int j);
   inline void col_to_col(int k, const Array<2,T> &mat, int j);


private:
   
   Array<0,T> *m_data;
};


template <typename T> inline
Array<2,T>::Array(int rows, int cols)
{
   if (rows > 0 && cols > 0) {
      m_data = new Array<0,T>(rows, cols, Array<0,T>::MATRIX);
   } else {
      m_data = nullptr;
   }
}


template <typename T> inline
Array<2,T>::Array(int rows, int cols, const T &value)
{
   if (rows > 0 && cols > 0) {
      m_data = new Array<0,T>(rows, cols, Array<0,T>::MATRIX);
      for (int k=0; k<size(); ++k) {
         at(k) = value;
      }
   } else {
      m_data = nullptr;
   }
}


template <typename T> inline
Array<2,T>::Array(const Array<2,T> &that)
{
   if (that.m_data != nullptr) {
      m_data = that.m_data->ref();
   } else {
      m_data = nullptr;
   }
}


template <typename T> inline
Array<2,T>::Array(Array<2,T> &&that)
{
   if (that.m_data != nullptr) {
      m_data = that.m_data->ref();
   } else {
      m_data = nullptr;
   }
}


template <typename T>
inline Array<2,T>::~Array()
{
   if (m_data != nullptr) {
      if (m_data->unref()) {
         delete m_data;
      }
   }
}


template <typename T> inline
Array<2,T>& Array<2,T>::operator= (const Array<2,T> &that)
{
   if (m_data != that.m_data) {
      if (m_data != nullptr) {
         if (m_data->unref()) {
            delete m_data;
         }
      }
      if (that.m_data != nullptr) {
         m_data = that.m_data->ref();
      } else {
         m_data = nullptr;
      }
   }
   return *this;
}


template <typename T> inline
Array<2,T>& Array<2,T>::operator= (Array<2,T> &&that)
{
   if (m_data != that.m_data) {
      if (m_data != nullptr) {
         if (m_data->unref()) {
            delete m_data;
         }
      }
      if (that.m_data != nullptr) {
         m_data = that.m_data->ref();
      } else {
         m_data = nullptr;
      }
   }
   return *this;
}


template <typename T> inline
void Array<2,T>::append(const T &value)
{
   if (m_data == nullptr) {
      m_data = new Array<0,T>(1, 1, Array<0,T>::MATRIX);
      at(0) = value;
   } else {
      if (m_data->m_rows > 1) {
          return;
      }
      m_data->append(value);
   }
}


template <typename T> inline
void Array<2,T>::reshape(int rows, int cols)
{
    if (m_data != nullptr) {
        if (rows*cols != m_data->size()) {
            return;
        }
        m_data->m_rows = rows;
        m_data->m_cols = cols;
    }
}


template <typename T> inline std::ostream&
operator<< (std::ostream &out, const Array<2,T> &v)
{
   int m = v.rows();
   int n = v.cols() - 1;
   
   if (m <= 0 || n < 0) {
      out << "[[]]";
      return out;
   }
   
   for (int i=0; i<m; ++i) {
      if (i == 0) out << "[[";
      else out << " [";
      for (int j=0; j<n; ++j) {
         out << v[i][j] << ", ";
      }
      if (i == m-1) out << v[i][n] << "]]";
      else out << v[i][n] << ']' << std::endl;
   }
   return out;
}


template <typename T> inline QDebug
operator<< (QDebug out, const Array<2,T> &v)
{
   int m = v.rows();
   int n = v.cols() - 1;
   
   if (m <= 0 || n < 0) {
      out << "[[]]";
      return out;
   }
   
   for (int i=0; i<m; ++i) {
      if (i == 0) out << "[[";
      else out << '[';
      for (int j=0; j<n; ++j) {
         out << v[i][j] << ", ";
      }
      if (i == m-1) out << v[i][n] << "]]";
      else out << v[i][n] << ']' << '\n';
   }
   return out;
}


template <typename T> inline
void Array<2,T>::set_col(int k, const T &value)
{
   for (int j=0; j<rows(); ++j) {
      (*this)[j][k] = value;
   }
}


template <typename T> inline
void Array<2,T>::set_col(int k, const Array<1,T> &vec)
{
   int minsize = qMin(rows(), vec.size());
   for (int j=0; j<minsize; ++j) {
      (*this)[j][k] = vec[j];
   }
}


template <typename T> inline
void Array<2,T>::set_row(int k, const T &value)
{
   for (int j=0; j<cols(); ++j) {
      (*this)[k][j] = value;
   }
}


template <typename T> inline
void Array<2,T>::set_row(int k, const Array<1,T> &vec)
{
   int minsize = qMin(cols(), vec.size());
   for (int j=0; j<minsize; ++j) {
      (*this)[k][j] = vec[j];
   }
}


template <typename T> inline
void Array<2,T>::row_to_row(int k, const Array<2,T> &mat, int p)
{
   int minsize = qMin(cols(), mat.cols());
   for (int j=0; j<minsize; ++j) {
      (*this)[k][j] = mat[p][j];
   }
}


template <typename T> inline
void Array<2,T>::col_to_row(int k, const Array<2,T> &mat, int p)
{
   int minsize = qMin(cols(), mat.rows());
   for (int j=0; j<minsize; ++j) {
      (*this)[k][j] = mat[j][p];
   }
}


template <typename T> inline
void Array<2,T>::row_to_col(int k, const Array<2,T> &mat, int p)
{
   int minsize = qMin(rows(), mat.cols());
   for (int j=0; j<minsize; ++j) {
      (*this)[j][k] = mat[p][j];
   }
}


template <typename T> inline
void Array<2,T>::col_to_col(int k, const Array<2,T> &mat, int p)
{
   int minsize = qMin(rows(), mat.rows());
   for (int j=0; j<minsize; ++j) {
      (*this)[j][k] = mat[j][p];
   }
}


template <typename T=double> inline
Array<2,T> zeros(int rows, int cols)
{
   Array<2,T> ret(rows, cols, T(0));
   return std::move(ret);
}


template <typename T=double> inline
Array<2,T> ones(int rows, int cols)
{
   Array<2,T> ret(rows, cols, T(1));
   return std::move(ret);
}


template <typename T=double> inline
Array<2,T> identity(int size, const T &factor=T(1))
{
   Array<2,T> ret(size, size);
   for (int i=0; i<size; ++i) {
      for (int j=0; j<size; ++j) {
         ret[i][j] = (i==j) ? factor : T(0);
      }
   }
   return std::move(ret);
}


template <typename T> inline
Array<2,T> samesize(const Array<2,T> &v)
{
   Array<2,T> ret(v.rows(), v.cols());
   return std::move(ret);
}


template <typename T=double> inline
Array<2,T> row_stack(std::initializer_list<Array<1,T>> init_list)
{
   int k = 0;
   for (auto &row : init_list) {
      if (row.size() > k) {
         k = row.size();
      }
   }
   
   Array<2,T> ret(init_list.size(), k);
   k = 0;
   for (auto &row : init_list) {
      int j = 0;
      for (const auto &x : row) {
         ret[k][j] = x;
         j += 1;
      }
      k += 1;
   }
   return std::move(ret);
}


template <typename T=double> inline
Array<2,T> column_stack(std::initializer_list<Array<1,T>> init_list)
{
   int k = 0;
   for (auto &col : init_list) {
      if (col.size() > k) {
         k = col.size();
      }
   }
   
   Array<2,T> ret(k, init_list.size());
   k = 0;
   for (auto &col : init_list) {
      int j = 0;
      for (const auto &x : col) {
         ret[j][k] = x;
         j += 1;
      }
      k += 1;
   }
   return std::move(ret);
}


/********************************************************
* Functions to copy rows and columns from matrices
*******************************************************/


template <typename T> inline
Array<1,T> row(const Array<2,T> &A, int j)
{
   Array<1,T> ret(A.cols());
   for (int k=0; k<ret.size(); ++k) {
      ret.at(k) = A[j][k];
   }
   return std::move(ret);
}


template <typename T> inline
Array<1,T> col(const Array<2,T> &A, int j)
{
   Array<1,T> ret(A.rows());
   for (int k=0; k<ret.size(); ++k) {
      ret.at(k) = A[k][j];
   }
   return std::move(ret);
}


/*****************************************************************
* Functions that can be applied to arrays of any dimension
****************************************************************/


template <int D, typename T>
inline Array<D,T> copy(const Array<D,T> &v)
{
   auto ret = samesize(v);
   for (int k=0; k<ret.size(); ++k) {
      ret.at(k) = v.at(k);
   }
   return std::move(ret);
}


template <typename Func, int D, typename T>
inline void apply(Func func, const Array<D,T> &v)
{
   for (int k=0; k<v.size(); ++k) {
      v.at(k) = func(v.at(k));
   }
}


template <typename Func, int D, typename T>
inline Array<D,T> applied(Func func, const Array<D,T> &v)
{
   auto y = samesize(v);
   for (int k=0; k<v.size(); ++k) {
      y.at(k) = func(v.at(k));
   }
   return std::move(y);
}


template <int D, typename T>
inline Array<D,T> sin(const Array<D,T> &v)
{ return applied(Math::sin, v); }


template <int D, typename T>
inline Array<D,T> cos(const Array<D,T> &v)
{ return applied(Math::cos, v); }

} // namespace Ksl

#endif // KSL_ARRAY_H
