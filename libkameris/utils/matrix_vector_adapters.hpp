#ifndef _LIBKAMERIS_UTILS_MATRIX_VECTOR_ADAPTERS_
#define _LIBKAMERIS_UTILS_MATRIX_VECTOR_ADAPTERS_

#include <cstddef>
#include <cstdlib>
#include <map>
#include <utility>

namespace kameris {
	template <typename T>
	class MatrixAdapter {
	 public:
		using value_type = T;

	 private:
		T *_data;
		size_t _rows = 0, _cols = 0;
		bool _own_data = false;

	 public:
		MatrixAdapter() = default;
		MatrixAdapter(T *data, size_t rows, size_t cols, bool own_data = false)
				: _data(data), _rows(rows), _cols(cols), _own_data(own_data) {}
		~MatrixAdapter() {
			if (_own_data) {
				delete[] _data;
			}
		}

		MatrixAdapter(const MatrixAdapter &other) : _data(other._data), _rows(other._rows), _cols(other._cols) {
			if (other._own_data) {
				throw std::logic_error("Copying is not allowed if the adapter owns the data");
			}
		}
		friend void swap(MatrixAdapter &first, MatrixAdapter &second) noexcept {
			using std::swap;

			swap(first._data, second._data);
			swap(first._rows, second._rows);
			swap(first._cols, second._cols);
			swap(first._own_data, second._own_data);
		}
		MatrixAdapter &operator=(const MatrixAdapter &other) {
			MatrixAdapter temp(other);
			swap(*this, temp);
			return *this;
		}
		MatrixAdapter(MatrixAdapter &&other) noexcept
				: _data(other._data), _rows(other._rows), _cols(other._cols), _own_data(other._own_data) {
			other._own_data = false; //to prevent the destructor call
		}
		MatrixAdapter &operator=(MatrixAdapter &&other) noexcept {
			swap(*this, other);
			return *this;
		}

		const T &operator()(size_t i, size_t j) const {
			return _data[(i * _cols) + j];
		}
		T &operator()(size_t i, size_t j) {
			return _data[(i * _cols) + j];
		}
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
		size_t rows() const {
			return _rows;
		}
		size_t cols() const {
			return _cols;
		}
		size_t size() const {
			return _rows * _cols;
		}
	};

	template <typename T>
	class VectorAdapter {
	 public:
		using value_type = T;

	 private:
		T *_data;
		size_t _size = 0;
		bool _own_data = false;

	 public:
		VectorAdapter() = default;
		VectorAdapter(T *data, size_t size, bool own_data = false) : _data(data), _size(size), _own_data(own_data) {}
		~VectorAdapter() {
			if (_own_data) {
				delete[] _data;
			}
		}

		VectorAdapter(const VectorAdapter &other) : _data(other._data), _size(other._size) {
			if (other._own_data) {
				throw std::logic_error("Copying is not allowed if the adapter owns the data");
			}
		}
		friend void swap(VectorAdapter &first, VectorAdapter &second) noexcept {
			using std::swap;

			swap(first._data, second._data);
			swap(first._size, second._size);
			swap(first._own_data, second._own_data);
		}
		VectorAdapter &operator=(VectorAdapter other) {
			VectorAdapter temp(other);
			swap(*this, temp);
			return *this;
		}
		VectorAdapter(VectorAdapter &&other) noexcept
				: _data(other._data), _size(other._size), _own_data(other._own_data) {
			other._own_data = false; //to prevent the destructor call
		}
		VectorAdapter &operator=(VectorAdapter &&other) noexcept {
			swap(*this, other);
			return *this;
		}

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
		size_t rows() const {
			return 1;
		}
		size_t cols() const {
			return _size;
		}
		size_t size() const {
			return _size;
		}
	};

	template <typename Key, typename T, template <typename, typename, typename...> class Map = std::map,
		typename... MapArgs>
	class SparseVectorAdapter {
	 public:
		using key_type = Key;
		using value_type = T;

	 private:
		Map<Key, T, MapArgs...> _data;
		size_t _length;

	 public:
		SparseVectorAdapter(Map<Key, T, MapArgs...> &&data, size_t length) : _data(data), _length(length) {}

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
		T *_data;
		size_t _size = 0;
		bool _own_data = false;

		//this is because we return always return /references/ to values, even those on the main diagonal
		mutable T default_value = T();

	 public:
		SymmetricDistanceMatrixAdapter() = default;
		SymmetricDistanceMatrixAdapter(T *data, size_t size, bool own_data = false)
				: _data(data), _size(size), _own_data(own_data) {}
		~SymmetricDistanceMatrixAdapter() {
			if (_own_data) {
				delete[] _data;
			}
		}

		SymmetricDistanceMatrixAdapter(const SymmetricDistanceMatrixAdapter &other)
				: _data(other._data), _size(other._size) {
			if (other._own_data) {
				throw std::logic_error("Copying is not allowed if the adapter owns the data");
			}
		}
		friend void swap(SymmetricDistanceMatrixAdapter &first, SymmetricDistanceMatrixAdapter &second) noexcept {
			using std::swap;

			swap(first._data, second._data);
			swap(first._size, second._size);
			swap(first._own_data, second._own_data);
		}
		SymmetricDistanceMatrixAdapter &operator=(const SymmetricDistanceMatrixAdapter &other) {
			SymmetricDistanceMatrixAdapter temp(other);
			swap(*this, temp);
			return *this;
		}
		SymmetricDistanceMatrixAdapter(SymmetricDistanceMatrixAdapter &&other) noexcept
				: _data(other._data), _size(other._size), _own_data(other._own_data) {
			other._own_data = false; //to prevent the destructor call
		}
		SymmetricDistanceMatrixAdapter &operator=(SymmetricDistanceMatrixAdapter &&other) noexcept {
			swap(*this, other);
			return *this;
		}

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
	inline MatrixAdapter<T> make_matrix_adapter(T *data, size_t rows, size_t cols, bool own_data = false) {
		return MatrixAdapter<T>(data, rows, cols, own_data);
	}

	template <typename T>
	inline VectorAdapter<T> make_vector_adapter(T *data, size_t size, bool own_data = false) {
		return VectorAdapter<T>(data, size, own_data);
	}

	template <typename Key, typename T, template <typename, typename, typename...> class Map = std::map,
		typename... MapArgs>
	inline SparseVectorAdapter<Key, T, Map, MapArgs...> make_sparse_vector_adapter(Map<Key, T, MapArgs...> data,
		/**/ size_t length) {
		return SparseVectorAdapter<Key, T, Map, MapArgs...>(std::move(data), length);
	}

	template <typename T>
	inline SymmetricDistanceMatrixAdapter<T> make_symmetric_distance_adapter(T *data, size_t size,
		/**/ bool own_data = false) {
		return SymmetricDistanceMatrixAdapter<T>(data, size, own_data);
	}
}

#endif
