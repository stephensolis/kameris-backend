#ifndef _LIBMMG_UTILS_MATRIX_VECTOR_ADAPTERS_
#define _LIBMMG_UTILS_MATRIX_VECTOR_ADAPTERS_

#include <cstddef>
#include <cstdlib>
#include <map>

namespace mmg {
	template <typename T>
	class MatrixAdapter {
	 public:
		using value_type = T;

	 private:
		T *const _data;
		const size_t _rows, _cols;

	 public:
		MatrixAdapter(T *data, size_t rows, size_t cols) : _data(data), _rows(rows), _cols(cols) {}

		const T &operator()(size_t i, size_t j) const {
			return _data[(i * _cols) + j];
		}
		T &operator()(size_t i, size_t j) {
			return _data[(i * _cols) + j];
		}
		const T *data() const {
			return _data;
		}
		T *data() {
			return _data;
		}
		size_t rows() const {
			return _rows;
		}
		size_t cols() const {
			return _cols;
		}
	};

	template <typename T>
	class VectorAdapter {
	 public:
		using value_type = T;

	 private:
		T *const _data;
		const size_t _size;

	 public:
		VectorAdapter(T *data, size_t size) : _data(data), _size(size) {}

		const T &operator[](size_t i) const {
			return _data[i];
		}
		T &operator[](size_t i) {
			return _data[i];
		}
		const T *data() const {
			return _data;
		}
		T *data() {
			return _data;
		}
		size_t size() const {
			return _size;
		}
	};

	template <typename Key, typename T, template <typename, typename, typename...> class Map = std::map,
		typename... MapArgs>
	class SparseVectorAdapter {
	 public:
		using key_type = typename Map<Key, T, MapArgs...>::key_type;
		using mapped_type = typename Map<Key, T, MapArgs...>::mapped_type;
		using value_type = typename Map<Key, T, MapArgs...>::value_type;

	 private:
		Map<Key, T, MapArgs...> &_data;
		const size_t _length;

	 public:
		SparseVectorAdapter(Map<Key, T, MapArgs...> &data, size_t length) : _data(data), _length(length) {}

		typename Map<Key, T, MapArgs...>::const_iterator begin() const {
			return _data.cbegin();
		}
		typename Map<Key, T, MapArgs...>::iterator begin() {
			return _data.begin();
		}
		typename Map<Key, T, MapArgs...>::const_iterator end() const {
			return _data.cend();
		}
		typename Map<Key, T, MapArgs...>::iterator end() {
			return _data.end();
		}
		const Map<Key, T, MapArgs...> &data() const {
			return _data;
		}
		Map<Key, T, MapArgs...> &data() {
			return _data;
		}
		size_t length() const {
			return _length;
		}
		size_t elements() const {
			return _data.size();
		}
	};

	template <typename T>
	class SymmetricDistanceMatrixAdapter {
	 public:
		using value_type = T;

	 private:
		T *const _data;
		const size_t _size;

		//this is because we return always return /references/ to values, even those on the main diagonal
		mutable T default_value = T();

	 public:
		SymmetricDistanceMatrixAdapter(T *data, size_t size) : _data(data), _size(size) {}

		const T &operator()(size_t i, size_t j) const {
			if (i < j) {
				return _data[(i * _size) + j - (i * (i + 3) / 2) - 1];
			} else if (i == j) {
				default_value = T(); //just in case the user tries to write to this value
				return default_value;
			} else {
				return _data[(j * _size) + i - (j * (j + 3) / 2) - 1];
			}
		}
		T &operator()(size_t i, size_t j) {
			return const_cast<T &>(const_cast<const SymmetricDistanceMatrixAdapter<T> &>(*this)(i, j)); // NOLINT
		}
		const T *data() const {
			return _data;
		}
		T *data() {
			return _data;
		}
		size_t rows() const {
			return _size;
		}
		size_t cols() const {
			return _size;
		}
	};

	template <typename T>
	inline MatrixAdapter<T> make_matrix_adapter(T *data, size_t rows, size_t cols) {
		return MatrixAdapter<T>(data, rows, cols);
	}

	template <typename T>
	inline VectorAdapter<T> make_vector_adapter(T *data, size_t size) {
		return VectorAdapter<T>(data, size);
	}

	template <typename Key, typename T, template <typename, typename, typename...> class Map = std::map,
		typename... MapArgs>
	inline SparseVectorAdapter<Key, T, Map, MapArgs...> make_sparse_vector_adapter(Map<Key, T, MapArgs...> &data,
		/**/ size_t length) {
		return SparseVectorAdapter<Key, T, Map, MapArgs...>(data, length);
	}

	template <typename T>
	inline SymmetricDistanceMatrixAdapter<T> make_symmetric_distance_adapter(T *data, size_t size) {
		return SymmetricDistanceMatrixAdapter<T>(data, size);
	}
}

#endif
