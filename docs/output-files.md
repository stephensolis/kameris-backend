# Output file formats

There are two different file formats used to store program output. `repr` mode generate files with the extension `.mm-repr` and `dist` mode generates files with the extension `.mm-dist`. This document describes these formats.

Data is always stored in little-endian format.

`element_type` is a common enum used to specify the type of the stored data. Its values are as follows:

| Value | Name      | Element size (bytes) | Description                              |
|-------|-----------|----------------------|------------------------------------------|
| 0     | `uint8`   | 1                    | 8-bit unsigned integer                   |
| 1     | `uint16`  | 2                    | 16-bit unsigned integer                  |
| 2     | `uint32`  | 4                    | 32-bit unsigned integer                  |
| 3     | `uint64`  | 8                    | 64-bit unsigned integer                  |
| 4     | `float32` | 4                    | IEEE 754 single-precision floating point |
| 5     | `float64` | 8                    | IEEE 754 double-precision floating point |

## `repr` format (`.mm-repr` files)

### Header

| Start | End  | Size (bytes)  | Name         | Description                                                              |
|-------|------|---------------|--------------|--------------------------------------------------------------------------|
| 0x00  | 0x05 | 6             | `magic`      | File signature, always "MMREPR"                                          |
| 0x06  | 0x06 | 1             | `version`    | Always 0                                                                 |
| 0x07  | 0x07 | 1             | `is_sparse`  | Whether the matrices are sparse (0 or 1)                                 |
| 0x08  | 0x08 | 1             | `key_type`   | The type of matrix keys as an `element_type`, unused if `is_sparse` is 0 |
| 0x09  | 0x09 | 1             | `value_type` | The type of matrix values as an `element_type`                           |
| 0x0A  | 0x11 | 8             | `count`      | Number of matrices                                                       |
| 0x12  | 0x19 | 8             | `rows`       | Number of rows for each matrix                                           |
| 0x1A  | 0x21 | 8             | `columns`    | Number of columns for each matrix                                        |

If the sparse flag is not set, the following `count` * `rows` * `columns` * (size for `value_type`) bytes are the entries, in row-major order, for each of the `count` matrices, one after the other.

If the sparse flag is set, each of the `count` matrices, one after the other, is stored as a 64-bit unsigned `size` followed by `size` * (size for `key_type`) * (size for `value_type`) bytes which are `size` pairs of key followed by value. The order of keys is unspecified.

## `dist` format (`.mm-dist` files)

### Header

| Start | End  | Size (bytes)  | Name         | Description                                    |
|-------|------|---------------|--------------|------------------------------------------------|
| 0x00  | 0x05 | 6             | `magic`      | File signature, always "MMDIST"                |
| 0x06  | 0x06 | 1             | `version`    | Always 0                                       |
| 0x07  | 0x07 | 1             | `value_type` | The type of matrix values as an `element_type` |
| 0x08  | 0x0F | 8             | `size`       | Number of rows = columns of the matrix         |

The following (`size` * (`size` - 1)) / 2 * (size for `value_type`) bytes are the entries **above** the main diagonal, in row-major order, of the matrix. The matrix is assumed to be symmetric and have zeros on the main diagonal.
