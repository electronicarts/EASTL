/////////////////////////////////////////////////////////////////////////////
// Copyright (c) Electronic Arts Inc. All rights reserved.
/////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
// *** Note ***
// This implementation is incomplete. 
//
// Additionally, this current implementation is not yet entirely in line with 
// EASTL conventions and thus may appear a little out of place to the observant.
// The goal is to bring thus file up to current standards in a future version.
///////////////////////////////////////////////////////////////////////////////


// To do:
// Remove forward declarations of classes.
// Remove mCol variable from matrix_cell.
// Make iterators have const and non-const versions.
// Remove mpCell from sparse_matrix_col_iterator.
// Remove mpRow from sparse_matrix_row_iterator.
// Remove mpMatrix from iterators.


///////////////////////////////////////////////////////////////////////////////
// This file implements a sparse matrix, which is a 2 dimensional array of 
// cells of an arbitrary type T. It is useful for situations where you need
// to store data in a very sparse way. The cost of storing an individual cell
// is higher than with a 2D array (or vector of vectors), but if the array is
// sparse, then a sparse matrix can save memory. It can also iterate non-empty
// cells faster than a regular 2D array, as only used cells are stored.
///////////////////////////////////////////////////////////////////////////////


#ifndef EASTL_SPARSE_MATRIX_H
#define EASTL_SPARSE_MATRIX_H

#if 0

#include <EASTL/internal/config.h>
#include <EASTL/map.h>
#include <EASTL/utility.h>
#include <EASTL/functional.h>

#if defined(EA_PRAGMA_ONCE_SUPPORTED)
	#pragma once // Some compilers (e.g. VC++) benefit significantly from using this. We've measured 3-4% build speed improvements in apps as a result.
#endif



namespace eastl
{

	// kRowColIndexNone
	// Refers to a row that is non-existant. If you call a function that returns a 
	// row or col index, and get kSparseMatrixIndexNone, the row or col doesn't exist.
	static const int32_t kSparseMatrixIntMin = (-2147483647 - 1);
	static const int32_t kSparseMatrixIntMax =   2147483647;


	///////////////////////////////////////////////////////////////////////////////
	// Forward declarations
	//
	template <typename T>                     struct matrix_cell;
	template <typename T, typename Allocator> struct matrix_row;
	template <typename T, typename Allocator> class  sparse_matrix;
	template <typename T, typename Allocator> class  sparse_matrix_row_iterator;
	template <typename T, typename Allocator> class  sparse_matrix_col_iterator;
	template <typename T, typename Allocator> class  sparse_matrix_iterator;



	///////////////////////////////////////////////////////////////////////////////
	/// matrix_cell
	///
	template <typename T>
	struct matrix_cell
	{
	public:
		typedef matrix_cell<T> this_type;
		typedef T              value_type;

	public:
		int         mCol;
		value_type  mValue;

	public:
		matrix_cell(int nCol = 0);
		matrix_cell(int nCol, const value_type& value);

	}; // matrix_cell



	///////////////////////////////////////////////////////////////////////////
	/// matrix_row
	///
	template <typename T, typename Allocator>
	struct matrix_row
	{
	public:
		typedef Allocator                                                    allocator_type;
		typedef matrix_row<T, allocator_type>                                this_type;
		typedef T                                                            value_type;
		typedef matrix_cell<T>                                               cell_type;
		typedef eastl::map<int, cell_type, eastl::less<int>, allocator_type> CellMap;

	public:
		int     mRow;
		CellMap mCellRow;

	public:
		matrix_row(int nRow = 0);

		// This function finds the given column in this row, if present. 
		// The result is a cell, and the pointer to the cell data itself 
		// is returned in the 'pCell' argument.
		bool GetMatrixCol(int nCol, cell_type*& pCell);

	}; // matrix_row




	///////////////////////////////////////////////////////////////////////////////
	/// sparse_matrix_row_iterator
	///
	/// Iterates cells in a given row of a sparse matrix.
	///
	template <typename T>
	class sparse_matrix_row_iterator
	{
	public:
		typedef sparse_matrix_row_iterator<T>           this_type;
		typedef eastl_size_t                            size_type;     // See config.h for the definition of eastl_size_t, which defaults to uint32_t.
		typedef ptrdiff_t                               difference_type;
		typedef T                                       value_type;
		typedef T&                                      reference;
		typedef T*                                      pointer;
		typedef EASTL_ITC_NS::forward_iterator_tag      iterator_category;

		typedef sparse_matrix<T, allocator_type>        MatrixType;
		typedef matrix_row<T, allocator_type>           row_type;
		typedef matrix_cell<T>                          cell_type;
		typedef eastl::map<int, row_type>               RowMap;
		typedef typename row_type::CellMap              CellMap;

	public:
		MatrixType*                mpMatrix;
		row_type*                  mpRow;
		typename CellMap::iterator mCellMapIterator;

	public:
		sparse_matrix_row_iterator(MatrixType* pMatrix, row_type* pRow, const typename CellMap::iterator& ic)
			: mpMatrix(pMatrix), mpRow(pRow), mCellMapIterator(ic)
		{
		}

		sparse_matrix_row_iterator(MatrixType* pMatrix = NULL)
			: mpMatrix(pMatrix), mpRow(NULL)
		{
		}

		int GetCol()        // Returns kSparseMatrixIntMin if iterator is 'empty'. We don't 
		{                   // return -1 because sparse matrix is not limited to rows/cols >= 0.
			if(mpRow)       // You can have a matrix that starts at column -100 and row -500.
			{
				const  cell_type& cell = (*mCellMapIterator).second;
				return cell.mCol;
			}
			return kSparseMatrixIntMin;
		}

		int GetRow()
		{
			if(mpRow)
				return mpRow->mRow;
			return kSparseMatrixIntMin;
		}

		bool operator==(const this_type& x) const
		{
			if(!mpRow && !x.mpRow)  // If we are comparing 'empty' iterators...
				return true;

			// The first check below wouldn't be necessary if we had a guarantee the iterators can compare between different rows.
			return (mpRow == x.mpRow) && (mCellMapIterator == x.mCellMapIterator);
		}

		bool operator!=(const this_type& x) const
		{ 
			return !operator==(x);
		}

		reference operator*() const
		{
			const cell_type& cell = (*mCellMapIterator).second;
			return cell.mValue; 
		}

		pointer operator->() const
		{
			const cell_type& cell = (*mCellMapIterator).second;
			return &cell.mValue; 
		}

		this_type& operator++()
		{
			++mCellMapIterator;
			return *this;
		}

		this_type operator++(int)
		{
			this_type tempCopy = *this;
			++*this;
			return tempCopy;
		}

	}; // sparse_matrix_row_iterator



	///////////////////////////////////////////////////////////////////////////////
	/// sparse_matrix_col_iterator
	///
	/// Iterates cells in a given column of a sparse matrix. Do not modify the 
	/// sparse_matrix while iterating through it. You can do this with some
	/// STL classes, but I'd rather not have to support this kind of code in 
	/// the future here.
	///
	template <typename T>
	class sparse_matrix_col_iterator
	{
	public:
		typedef sparse_matrix_col_iterator<T>           this_type;
		typedef eastl_size_t                            size_type;     // See config.h for the definition of eastl_size_t, which defaults to uint32_t.
		typedef ptrdiff_t                               difference_type;
		typedef T                                       value_type;
		typedef T&                                      reference;
		typedef T*                                      pointer;
		typedef EASTL_ITC_NS::forward_iterator_tag      iterator_category;

		typedef sparse_matrix<T, allocator_type>        MatrixType;
		typedef matrix_row<T, allocator_type>           row_type;
		typedef matrix_cell<T>                          cell_type;
		typedef eastl::map<int, row_type>               RowMap;
		typedef typename row_type::CellMap              CellMap;

	public:
		MatrixType*               mpMatrix;
		typename RowMap::iterator mRowMapIterator;
		cell_type*                mpCell;

	public:
		sparse_matrix_col_iterator(MatrixType* pMatrix, const typename RowMap::iterator& i, cell_type* pCell)
			: mpMatrix(pMatrix), mRowMapIterator(i), mpCell(pCell)
		{
		}

		sparse_matrix_col_iterator(MatrixType* pMatrix = NULL)
			: mpMatrix(pMatrix), mpCell(NULL)
		{
		}

		int GetCol()                    // Returns kSparseMatrixIntMin if iterator is 'empty'. We don't return -1
		{                               // because sparse matrix is not limited to rows/cols >= 0.
			if(mpCell)                  // You can have a matrix that starts at column -100 and row -500.
				return mpCell->mCol;
			return kSparseMatrixIntMin;
		}

		int GetRow()
		{
			if(mpCell)                                     // This might look strange, but we are using 'pCell' to 
				return (*mRowMapIterator).second.mRow;     // simply tell us if the iterator is 'empty' or not.
			return kSparseMatrixIntMin;
		}

		bool operator==(const this_type& x) const
		{
			if(!mpCell && !x.mpCell)    // If we are comparing 'empty' iterators...
				return true;

			// The second check below wouldn't be necessary if we had a guarantee the iterators can compare between different maps.
			return (mRowMapIterator == x.mRowMapIterator) && (mpCell == x.mpCell);
		}

		bool operator!=(const this_type& x) const
		{ 
			return !operator==(x);
		}

		reference operator*() const
		{                   
			return mpCell->mValue; 
		}

		reference operator->() const
		{                   
			return &mpCell->mValue; 
		}

		this_type& operator++()
		{                        
			++mRowMapIterator;

			while(mRowMapIterator != mpMatrix->mRowMap.end())
			{
				row_type& row = (*mRowMapIterator).second;

				// Can't we just use row.mCellRow.find(cell)?
				typename CellMap::const_iterator it = row.mCellRow.find(mpCell->mCol);

				if(it != row.mCellRow.end())
				{
					mpCell = const_cast<cell_type*>(&(*it).second); // Trust me, we won't be modifying the data.
					return *this;
				}

				// Linear search:
				//for(typename CellMap::iterator it(row.mCellRow.begin()); it != row.mCellRow.end(); ++it)
				//{
				//    const cell_type& cell = (*it).second;
				//
				//    if(cell.mCol == mpCell->mCol)
				//    {
				//        mpCell = const_cast<cell_type*>(&cell); // Trust me, we won't be modifying the data.
				//        return *this;
				//    }
				//}

				++mRowMapIterator;
			}

			mpCell = NULL;
			return *this;
		}

		this_type operator++(int)
		{                     
			this_type tempCopy = *this;
			++*this;
			return tempCopy;
		}

	}; // sparse_matrix_col_iterator



	///////////////////////////////////////////////////////////////////////////////
	/// sparse_matrix_iterator
	///
	/// Iterates cells of a sparse matrix, by rows and columns. Each row is iterated
	/// and each column within that row is iterated in order.
	///
	template <typename T>
	class sparse_matrix_iterator
	{
	public:
		typedef sparse_matrix_iterator<T>               this_type;
		typedef eastl_size_t                            size_type;     // See config.h for the definition of eastl_size_t, which defaults to uint32_t.
		typedef ptrdiff_t                               difference_type;
		typedef T                                       value_type;
		typedef T&                                      reference;
		typedef T*                                      pointer;
		typedef EASTL_ITC_NS::forward_iterator_tag      iterator_category;

		typedef sparse_matrix<T, allocator_type>        MatrixType;
		typedef matrix_row<T, allocator_type>           row_type;
		typedef matrix_cell<T>                          cell_type;
		typedef eastl::map<int, row_type>               RowMap;
		typedef typename row_type::CellMap              CellMap;

	public:
		MatrixType*                mpMatrix;
		typename RowMap::iterator  mRowMapIterator;
		typename CellMap::iterator mCellMapIterator;

	public:
		sparse_matrix_iterator(MatrixType* pMatrix, const typename RowMap::iterator& ir, const typename CellMap::iterator& ic)
			: mpMatrix(pMatrix), mRowMapIterator(ir), mCellMapIterator(ic)
		{
		}

		sparse_matrix_iterator(MatrixType* pMatrix, const typename RowMap::iterator& ir)
			: mpMatrix(pMatrix), mRowMapIterator(ir), mCellMapIterator()
		{
		}

		int GetCol()
		{
			const cell_type& cell = (*mCellMapIterator).second;
			return cell.mCol;
		}

		int GetRow()
		{
			const row_type& row = (*mRowMapIterator).second;
			return row.mRow;
		}

		bool operator==(const this_type& x) const
		{
			return (mRowMapIterator == x.mRowMapIterator) && (mCellMapIterator == x.mCellMapIterator);
		}

		bool operator!=(const this_type& x) const
		{ 
			return (mRowMapIterator != x.mRowMapIterator) || (mCellMapIterator != x.mCellMapIterator);
		}

		reference operator*() const
		{
			cell_type& cell = (*mCellMapIterator).second;
			return cell.mValue; 
		}

		this_type& operator++()
		{
			++mCellMapIterator; // Increment the current cell (column) in the current row.

			row_type& row = (*mRowMapIterator).second;

			if(mCellMapIterator == row.mCellRow.end()) // If we hit the end of the current row...
			{
				++mRowMapIterator;

				while(mRowMapIterator != mpMatrix->mRowMap.end()) // While we haven't hit the end of rows...
				{
					row_type& row = (*mRowMapIterator).second;

					if(!row.mCellRow.empty()) // If there are any cells (columns) in this row...
					{
						mCellMapIterator = row.mCellRow.begin();
						break;
					}

					++mRowMapIterator;
				}
			}

			return *this;
		}

		this_type operator++(int)
		{
			this_type tempCopy = *this;
			operator++();
			return tempCopy;
		}

	}; // sparse_matrix_iterator



	///////////////////////////////////////////////////////////////////////////////
	/// sparse_matrix
	///
	template <typename T*, typename Allocator = EASTLAllocatorType>
	class sparse_matrix
	{
	public:
		typedef sparse_matrix<T, Allocator>                 this_type;
		typedef T                                           value_type;
		typedef value_type*                                 pointer;
		typedef const value_type*                           const_pointer;
		typedef value_type&                                 reference;
		typedef const value_type&                           const_reference;
		typedef ptrdiff_t                                   difference_type;
		typedef eastl_size_t                                size_type;     // See config.h for the definition of eastl_size_t, which defaults to uint32_t.
		typedef sparse_matrix_row_iterator<T>               row_iterator;
		typedef sparse_matrix_col_iterator<T>               col_iterator;
		typedef sparse_matrix_iterator<T>                   iterator;
		typedef sparse_matrix_iterator<T>                   const_iterator; // To do: Fix this.
		typedef Allocator                                   allocator_type;
		typedef matrix_row<T, Allocator>                    row_type; 
		typedef typename row_type::CellMap                  CellMap;
		typedef eastl::map<int, row_type, allocator_type>   RowMap;

		// iterator friends
		friend class sparse_matrix_row_iterator<T>;
		friend class sparse_matrix_col_iterator<T>;
		friend class sparse_matrix_iterator<T>;

		// kRowColIndexNone
		static const int32_t kRowColIndexNone = kSparseMatrixIntMin;

		// UserCell
		// We don't internally use this struct to store data, because that would
		// be inefficient. However, whenever the user of this class needs to query for
		// individual cells, especially in batches, it is useful to have a struct that
		// identifies both the cell coordinates and cell data for the user.
		struct UserCell     
		{
			int mCol;
			int mRow;
			T   mValue;
		};

	public:
		sparse_matrix();
		sparse_matrix(const sparse_matrix& x);
	   ~sparse_matrix();

		this_type& operator=(const this_type& x);

		void swap();

		// Iterators
		row_iterator row_begin(int nRow);
		row_iterator row_end(int nRow);
		col_iterator col_begin(int nCol);
		col_iterator col_end(int nCol);
		iterator     begin();
		iterator     end();

		// Standard interface functions
		bool        empty() const;                                                           // Returns true if no cells are used.
		size_type   size() const;                                                            // Returns total number of non-empty cells.

		int         GetMinUsedRow(int& nResultCol) const;                                    // Returns first row that has data. Fills in column that has that data. Returns kRowUnused if no row has data.
		int         GetMaxUsedRow(int& nResultCol) const;                                    // Returns last row that has data. Fills in column that has that data. Returns kRowUnused if no row has data.
		bool        GetMinMaxUsedColForRow(int nRow, int& nMinCol, int& nMaxCol) const;      // Sets the min and max column and returns true if any found.
		bool        GetMinMaxUsedRowForCol(int nCol, int& nMinRow, int& nMaxRow) const;      // Sets the min and max row and returns true if any found.
		size_type   GetColCountForRow(int nRow) const;                                       // You specify the row, it gives you the used cell count.

		int         GetMinUsedCol(int& nResultRow) const;                                    // Returns first column that has data. Fills in row that has that data. Returns kColUnused if no column has data.
		int         GetMaxUsedCol(int& nResultRow) const;                                    // Returns last column that has data. Fills in row that has that data. Returns kColUnused if no column has data.
		size_type   GetRowCountForCol(int nCol) const;                                       //
		int         GetRowWithMaxColCount(size_type& nColCount) const;                       //

		bool        remove(int nRow, int nCol, T* pPreviousT = NULL);                        // If you pass in a 'pPreviousT', it will copy in value to it before removing the cell.
		bool        remove_row(int nRow, size_type nCount = 1);                              // Removes 'nCount' rows, starting at 'nRow'.
		bool        remove_col(int nCol, size_type nCount = 1);                              // Removes 'nCount' cols, starting at 'nCol'.
		bool        clear();                                                                 // Removes all cells.
		void        insert(int nRow, int nCol, const value_type& t, value_type* pPrevValue = NULL);            // If you pass in a 'pPreviousT', it will copy in value to it before changing the cell.
		bool        IsCellUsed(int nRow, int nCol);                                          // Returns true if cell is non-empty

		bool        GetCell(int nRow, int nCol, value_type* pValue = NULL);                  //
		bool        GetCellPtr(int nRow, int nCol, value_type** pValue);                     // Gets a pointer to the cell itself, for direct manipulation.
		size_type   GetCellCountForRange(int nRowStart, int nRowEnd,
										 int nColStart, int nColEnd);                        // Counts cells in range. Range is inclusive.
		int         GetCellRange(int nRowStart, int nRowEnd, 
								 int nColStart, int nColEnd, UserCell* pCellArray = NULL);   // Copies cell data into the array of UserCells provided by the caller.
		int         FindCell(const value_type& t, UserCell* pCellArray = NULL);              // Finds all cells that match the given argument cell. Call this function with NULL pCellArray to simply get the count.

		bool  validate();
		int   validate_iterator(const_iterator i) const;

	protected:
		bool GetMatrixRow(int nRow, row_type*& pRow);

	protected:
		RowMap         mRowMap;     /// Map of all row data. It is a map of maps.
		size_type      mnSize;      /// The count of all cells. This is equal to the sums of the sizes of the maps in mRowMap.
		allocator_type mAllocator;  /// The allocator for all data.

	}; // sparse_matrix







	///////////////////////////////////////////////////////////////////////////////
	// matrix_cell
	///////////////////////////////////////////////////////////////////////////////

	template <typename T>
	matrix_cell<T>::matrix_cell(int nCol = 0) 
		: mCol(nCol), mValue()
	{
	}

	template <typename T>
	matrix_cell<T>::matrix_cell(int nCol, const value_type& value) 
		: mCol(nCol), mValue(value)
	{
	}




	///////////////////////////////////////////////////////////////////////////////
	// matrix_row
	///////////////////////////////////////////////////////////////////////////////

	template <typename T, typename Allocator>
	matrix_row<T>::matrix_row(int nRow = 0)
		: mRow(nRow), mCellRow()
	{
	}

	template <typename T, typename Allocator>
	bool matrix_row<T>::GetMatrixCol(int nCol, cell_type*& pCell)
	{
		#if EASTL_ASSERT_ENABLED
			int nPreviousCol(sparse_matrix<T, Allocator>::kRowColIndexNone);
			EASTL_ASSERT((nCol < kSparseMatrixIntMax / 2) && (nCol > kSparseMatrixIntMin / 2));
		#endif

		typename CellMap::iterator it(mCellRow.find(nCol));

		if(it != mCellRow.end())
		{
			cell_type& cell = (*it).second;
			pCell = &cell;
			return true;
		}

		return false;
	}

	template <typename T, typename Allocator>
	inline bool operator==(const matrix_row<T, Allocator>& a, const matrix_row<T, Allocator>& b)
	{ 
		return (a.mRow == b.mRow) && (a.mCellRow == b.mCellRow);
	}

	template <typename T, typename Allocator>
	inline bool operator==(const matrix_cell<T, Allocator>& a, const matrix_cell<T, Allocator>& b)
	{ 
		return (a.mValue == b.mValue);
	}




	///////////////////////////////////////////////////////////////////////////////
	// sparse_matrix
	///////////////////////////////////////////////////////////////////////////////

	template <typename T, typename Allocator>
	inline sparse_matrix<T, Allocator>::sparse_matrix()
		: mRowMap(), mnSize(0)
	{
	}


	template <typename T, typename Allocator>
	inline sparse_matrix<T, Allocator>::sparse_matrix(const this_type& x)
	{
		mnSize  = x.mnSize;
		mRowMap = x.mRowMap;
	}


	template <typename T, typename Allocator>
	inline sparse_matrix<T, Allocator>::~sparse_matrix()
	{
		// Nothing to do.
	}


	template <typename T, typename Allocator>
	inline typename sparse_matrix<T, Allocator>::this_type&
	sparse_matrix<T, Allocator>::operator=(const this_type& x)
	{
		// Check for self-asignment is not needed, as the assignments below already do it.
		mnSize  = x.mnSize;
		mRowMap = x.mRowMap;
		return *this;
	}


	template <typename T, typename Allocator>
	inline void sparse_matrix<T>& sparse_matrix<T, Allocator>::swap()
	{
		eastl::swap(mnSize,  x.mnSize);
		eastl::swap(mRowMap, x.mRowMap);
	}


	template <typename T, typename Allocator>
	inline bool sparse_matrix<T, Allocator>::empty() const
	{
		return (mnSize == 0);
	}


	template <typename T, typename Allocator>
	inline typename sparse_matrix<T, Allocator>::size_type
	sparse_matrix<T, Allocator>::size() const
	{
		return mnSize;
	}


	///////////////////////////////////////////////////////////////////////////////
	// row_begin
	//
	// This function returns a sparse matrix row iterator. It allows you to 
	// iterate all used cells in a given row. You pass in the row index and it 
	// returns an iterator for the first used cell. You can dereference the 
	// iterator to get the cell data. Just like STL containers, the end iterator
	// is one-past the past the last valid iterator. A row iterator returned
	// by this function is good only for that row; likewise, you can only use
	// such a row iterator with the end iterator for that row and not with an
	// end iterator for any other row.
	//
	// Here is an example of using a row iterator to iterate all used cells
	// in row index 3 of a sparse matrix of 'int':
	//     sparse_matrix<int>::row_iterator it    = intMatrix.row_begin(3);
	//     sparse_matrix<int>::row_iterator itEnd = intMatrix.row_end(3);
	//
	//     while(it != itEnd)
	//     {
	//         printf("Col=%d, row=%d, value=%d\n", it.GetCol(), it.GetRow(), *it);
	//         ++it;
	//     }
	//
	template <typename T, typename Allocator>
	typename sparse_matrix<T, Allocator>::row_iterator
	sparse_matrix<T, Allocator>::row_begin(int nRow)
	{
		EASTL_ASSERT((nRow < kSparseMatrixIntMax / 2) && (nRow > kSparseMatrixIntMin / 2));

		row_type* pRow;

		if(GetMatrixRow(nRow, pRow))
			return sparse_matrix_row_iterator<T>(this, pRow, pRow->mCellRow.begin());
		return sparse_matrix_row_iterator<T>(this); //Create an 'empty' iterator.
	}


	///////////////////////////////////////////////////////////////////////////////
	// row_end
	//
	// Returns the end iterator for a given row. See the row_begin function for more.
	//
	template <typename T, typename Allocator>
	inline typename sparse_matrix<T, Allocator>::row_iterator
	sparse_matrix<T, Allocator>::row_end(int nRow)
	{
		EASTL_ASSERT((nRow < kSparseMatrixIntMax / 2) && (nRow > kSparseMatrixIntMin / 2));

		row_type* pRow;

		if(GetMatrixRow(nRow, pRow))
			return sparse_matrix_row_iterator<T>(this, pRow, pRow->mCellRow.end());
		return sparse_matrix_row_iterator<T>(this); //Create an 'empty' iterator.
	}


	///////////////////////////////////////////////////////////////////////////////
	// col_begin
	//
	// This function returns a sparse matrix column iterator. A column iterator 
	// acts just like a row iterator except it iterates cells in a column instead
	// of cells in a row.
	//
	// Here is an example of using a column iterator to iterate all used cells
	// in column index 0 (the first column) of a sparse matrix of 'int':
	//     sparse_matrix<int>::col_iterator it     = intMatrix.col_begin(0);
	//     sparse_matrix<int>::col_iterator itEnd = intMatrix.col_end(0);
	//
	//     while(it != itEnd)
	//     {
	//         printf("Col=%d, row=%d, value=%d\n", it.GetCol(), it.GetRow(), *it);
	//         ++it;
	//     }
	//
	template <typename T, typename Allocator>
	typename sparse_matrix<T, Allocator>::col_iterator
	sparse_matrix<T, Allocator>::col_begin(int nCol)
	{
		EASTL_ASSERT((nCol < kSparseMatrixIntMax / 2) && (nCol > kSparseMatrixIntMin / 2));

		for(typename RowMap::iterator it(mRowMap.begin()); it != mRowMap.end(); ++it)
		{
			const row_type& matrixRowConst = (*it).second;
			row_type&       row            = const_cast<row_type&>(matrixRowConst);

			for(typename CellMap::iterator it1(row.mCellRow.begin()); it1!=row.mCellRow.end(); ++it1)
			{
				const cell_type& cellConst = (*it1).second;
				cell_type&       cell      = const_cast<cell_type&>(cellConst);

				if(cell.mCol == nCol)
					return sparse_matrix_col_iterator<T>(this, it, &cell);
			}
		}
		return sparse_matrix_col_iterator<T>(this, mRowMap.end(), NULL);
	}


	///////////////////////////////////////////////////////////////////////////////
	// col_end
	//
	// Returns the end iterator for a given colum. See the col_begin function for more.
	//
	template <typename T, typename Allocator>
	inline typename sparse_matrix<T, Allocator>::col_iterator
	sparse_matrix<T, Allocator>::col_end(int nCol)
	{
		EASTL_ASSERT((nCol < kSparseMatrixIntMax / 2) && (nCol > kSparseMatrixIntMin / 2));

		return sparse_matrix_col_iterator<T>(this, mRowMap.end(), NULL);
	}


	///////////////////////////////////////////////////////////////////////////////
	// begin
	//
	// This function returns a sparse matrix cell iterator. It iterates all used
	// cells in the sparse matrix. The cells are returned in column,row order 
	// (as opposed to row,column order). Thus, all columns for a given row will
	// be iterated before moving onto the next row.
	//
	// Here is an example of using an iterator to iterate all used cells:
	//     sparse_matrix<int>::iterator it    = intMatrix.begin();
	//     sparse_matrix<int>::iterator itEnd = intMatrix.end();
	//
	//     while(it != itEnd)
	//     {
	//         printf("Col=%d, row=%d, value=%d\n", it.GetCol(), it.GetRow(), *it);
	//         ++it;
	//     }
	//
	template <typename T, typename Allocator>
	typename sparse_matrix<T, Allocator>::iterator
	sparse_matrix<T, Allocator>::begin()
	{
		for(typename RowMap::iterator it(mRowMap.begin()); it != mRowMap.end(); ++it)
		{
			row_type& row = (*it).second;

			if(!row.mCellRow.empty())
				return sparse_matrix_iterator<T>(this, it, row.mCellRow.begin());
		}
		return sparse_matrix_iterator<T>(this, mRowMap.end());
	}


	template <typename T, typename Allocator>
	inline typename sparse_matrix<T, Allocator>::iterator
	sparse_matrix<T, Allocator>::end()
	{
		return sparse_matrix_iterator<T>(this, mRowMap.end());
	}


	template <typename T, typename Allocator>
	int sparse_matrix<T, Allocator>::GetMinUsedRow(int& nResultCol) const
	{
		if(!mRowMap.empty())
		{
			const row_type&  row  = (*mRowMap.begin()).second;        // Get the last row.
			const cell_type& cell = (*row.mCellRow.begin()).second;   // Get the first cell in that row, though it doesn't really matter which one we get.

			nResultCol = cell.mCol;

			return row.mRow; // Return the row of the last item in the map.
		}

		nResultCol = kRowColIndexNone;
		return kRowColIndexNone;
	}


	template <typename T, typename Allocator>
	int sparse_matrix<T, Allocator>::GetMaxUsedRow(int& nResultCol) const
	{
		if(!mRowMap.empty())
		{
			const row_type&  row  = (*mRowMap.rbegin()).second;        // Get the last row.
			const cell_type& cell = (*row.mCellRow.begin()).second;    // Get the first cell in that row, though it doesn't really matter which one we get.

			nResultCol = cell.mCol;

			return row.mRow; // Return the row of the last item in the map.
		}

		nResultCol = kRowColIndexNone;
		return kRowColIndexNone;
	}


	template <typename T, typename Allocator>
	bool sparse_matrix<T, Allocator>::GetMinMaxUsedColForRow(int nRow, int& nMinCol, int& nMaxCol) const
	{
		bool bReturnValue(false);

		EASTL_ASSERT((nRow < kSparseMatrixIntMax / 2) && (nRow > kSparseMatrixIntMin / 2));

		nMinCol = kSparseMatrixIntMax;
		nMaxCol = kSparseMatrixIntMin;

		typename RowMap::iterator it(mRowMap.find(nRow));

		if(it != mRowMap.end())
		{
			const row_type& row = (*it).second;
			EASTL_ASSERT(!row.mCellRow.empty()); // All rows should have at least one col, or we would have removed it.

			const cell_type& matrixCellFront = (*row.mCellRow.begin()).second;
			const cell_type& matrixCellBack  = (*row.mCellRow.rbegin()).second;

			nMinCol = matrixCellFront.mCol;
			nMaxCol = matrixCellBack.mCol;

			bReturnValue = true;
		}

		return bReturnValue;
	}


	///////////////////////////////////////////////////////////////////////////////
	// GetMinMaxUsedRowForCol
	//
	template <typename T, typename Allocator>
	bool sparse_matrix<T, Allocator>::GetMinMaxUsedRowForCol(int nCol, int& nMinRow, int& nMaxRow) const
	{
		// The implementation of this function is a little tougher than with the "col for row" version of
		// this function, since the data is stored in row maps instead of column maps.
		bool bReturnValue(false);

		EASTL_ASSERT((nCol < kSparseMatrixIntMax / 2) && (nCol > kSparseMatrixIntMin / 2));

		nMinRow = kSparseMatrixIntMax;
		nMaxRow = kSparseMatrixIntMin;

		//First search for the min row.
		for(typename RowMap::iterator it(mRowMap.begin()); it != mRowMap.end(); ++it)
		{
			row_type& row = (*it).second;
			EASTL_ASSERT(!row.mCellRow.empty()); // All rows should have at least one col, or we would have removed the row.

			// Find the given column in this row. If present work on it.
			typename CellMap::iterator it1(row.mCellRow.find(nCol));

			if(it1 != row.mCellRow.end())
			{
				nMinRow = row.mRow;
				nMaxRow = row.mRow;
				bReturnValue = true;
				break;
			}
		}

		// Now search for a max row.
		if(bReturnValue) // There can only be a max row if there was also a min row.
		{
			for(typename RowMap::reverse_iterator it(mRowMap.rbegin()); it != mRowMap.rend(); ++it)
			{
				row_type& row = (*it).second;
				EASTL_ASSERT(!row.mCellRow.empty()); // All rows should have at least one col, or we would have removed the row.

				// Find the given column in this row. If present work on it.
				typename CellMap::iterator it1(row.mCellRow.find(nCol));

				if(it1 != row.mCellRow.end())
				{
					nMaxRow = row.mRow;
					break;
				}
			}
		}

		return bReturnValue;
	}


	///////////////////////////////////////////////////////////////////////////////
	// GetColCountForRow
	//
	template <typename T, typename Allocator>
	typename sparse_matrix<T, Allocator>::size_type
	sparse_matrix<T, Allocator>::GetColCountForRow(int nRow) const
	{
		EASTL_ASSERT((nRow < kSparseMatrixIntMax / 2) && (nRow > kSparseMatrixIntMin / 2));

		row_type* pRow;

		if(GetMatrixRow(nRow, pRow))
			return (size_type)pRow->mCellRow.size();
		return 0;
	}


	///////////////////////////////////////////////////////////////////////////////
	// GetMinUsedCol
	//
	template <typename T, typename Allocator>
	int sparse_matrix<T, Allocator>::GetMinUsedCol(int& nResultRow) const
	{
		int nMinCol = kRowColIndexNone;
		nResultRow  = kRowColIndexNone;

		for(typename RowMap::iterator it(mRowMap.begin()); it != mRowMap.end(); ++it)
		{
			row_type& row = (*it).second;
			EASTL_ASSERT(!row.mCellRow.empty()); // All rows should have at least one col, or we would have removed it.

			const cell_type& cell = (*row.mCellRow.begin()).second;

			if((cell.mCol < nMinCol) || (nMinCol == kRowColIndexNone))
			{
				nMinCol    = cell.mCol;
				nResultRow = row.mRow;
			}
		}

		return nMinCol;
	}


	///////////////////////////////////////////////////////////////////////////////
	// GetMaxUsedCol
	//
	template <typename T, typename Allocator>
	int sparse_matrix<T, Allocator>::GetMaxUsedCol(int& nResultRow) const
	{
		int nMaxCol = kRowColIndexNone;
		nResultRow  = kRowColIndexNone;

		for(typename RowMap::iterator it(mRowMap.begin()); it != mRowMap.end(); ++it)
		{
			row_type& row = (*it).second;
			EASTL_ASSERT(!row.mCellRow.empty()); // All rows should have at least one col, or we would have removed it.

			const cell_type& cell = (*row.mCellRow.rbegin()).second;

			if((cell.mCol > nMaxCol) || (nMaxCol == kRowColIndexNone))
			{
				nMaxCol    = cell.mCol;
				nResultRow = row.mRow;
			}
		}

		return nMaxCol;
	}


	///////////////////////////////////////////////////////////////////////////////
	// GetRowCountForCol
	//
	template <typename T, typename Allocator>
	typename sparse_matrix<T, Allocator>::size_type
	sparse_matrix<T, Allocator>::GetRowCountForCol(int nCol) const
	{
		EASTL_ASSERT((nCol < kSparseMatrixIntMax / 2) && (nCol > kSparseMatrixIntMin / 2));

		size_type nRowCount = 0;

		for(typename RowMap::iterator it(mRowMap.begin()); it != mRowMap.end(); ++it)
		{
			row_type& row = (*it).second;
			EASTL_ASSERT(!row.mCellRow.empty()); 

			//Faster set-based code:
			typename CellMap::iterator it1(row.mCellRow.find(nCol));
			if(it1 != row.mCellRow.end())
				nRowCount++;
		}

		return nRowCount;
	}


	///////////////////////////////////////////////////////////////////////////////
	// GetRowWithMaxColCount
	//
	template <typename T, typename Allocator>
	int sparse_matrix<T, Allocator>::GetRowWithMaxColCount(size_type& nColCount) const
	{
		int nRow  = 0;
		nColCount = 0;

		for(typename RowMap::iterator it(mRowMap.begin()); it != mRowMap.end(); ++it)
		{
			const row_type& row = (*it).second;
			const size_type nSize(row.mCellRow.size());
			EASTL_ASSERT(nSize != 0);

			if(nSize > (size_type)nColCount)
			{
				nRow      = row.mRow;
				nColCount = nSize;
			}
		}
		return nRow;
	}


	///////////////////////////////////////////////////////////////////////////
	// GetCellCountForRange
	//
	template <typename T, typename Allocator>
	typename sparse_matrix<T, Allocator>::size_type
	sparse_matrix<T, Allocator>::GetCellCountForRange(int nRowStart, int nRowEnd, int nColStart, int nColEnd) const
	{
		size_type nCellCount(0);

		// Note by Paul P.: This could be made a little faster by doing a search 
		// for the first row and iterating the container from then on.
		for(typename RowMap::iterator it(mRowMap.begin()); it != mRowMap.end(); ++it)
		{
			row_type& row = (*it).second;

			if(row.mRow < nRowStart)
				continue;

			if(row.mRow > nRowEnd)
				break;

			for(typename CellMap::iterator it1(row.mCellRow.begin()); it1 != row.mCellRow.end(); ++it1)
			{
				const cell_type& cell = (*it1).second;

				if(cell.mCol < nColStart)
					continue;

				if(cell.mCol > nColEnd)
					break;

				nCellCount++;
			}
		}

		return nCellCount;
	}


	///////////////////////////////////////////////////////////////////////////////
	// GetCellRange
	//
	template <typename T, typename Allocator>
	int sparse_matrix<T, Allocator>::GetCellRange(int nRowStart, int nRowEnd,
												  int nColStart, int nColEnd, UserCell* pCellArray) const
	{
		int nCellCount(0);

		// Note by Paul P.: This could be made a little faster by doing a search 
		// for the first row and iterating the container from then on.

		for(typename RowMap::iterator it(mRowMap.begin()); it != mRowMap.end(); ++it)
		{
			row_type& row = (*it).second;

			if(row.mRow < nRowStart)
				continue;
			if(row.mRow > nRowEnd)
				break;

			for(typename CellMap::iterator it1(row.mCellRow.begin()); it1 != row.mCellRow.end(); ++it1)
			{
				const cell_type& cell = (*it1).second;

				if(cell.mCol < nColStart)
					continue;

				if(cell.mCol > nColEnd)
					break;

				if(pCellArray)
				{
					pCellArray[nCellCount].mCol = cell.mCol;
					pCellArray[nCellCount].mRow = row.mRow;
					pCellArray[nCellCount].mValue   = cell.mValue;
				}

				nCellCount++;
			}
		}

		return nCellCount;
	}


	///////////////////////////////////////////////////////////////////////////////
	// remove
	//
	template <typename T, typename Allocator>
	bool sparse_matrix<T, Allocator>::remove(int nRow, int nCol, T* pPreviousT)
	{
		EASTL_ASSERT((nCol < kSparseMatrixIntMax / 2) && (nCol > kSparseMatrixIntMin / 2));
		EASTL_ASSERT((nRow < kSparseMatrixIntMax / 2) && (nRow > kSparseMatrixIntMin / 2));

		//Faster map-based technique:
		typename RowMap::iterator it(mRowMap.find(nRow));

		if(it != mRowMap.end())
		{
			row_type& row = (*it).second;

			typename CellMap::iterator it1(row.mCellRow.find(nCol));

			if(it1 != row.mCellRow.end())
			{
				cell_type& cell = (*it1).second;

				if(pPreviousT)
					*pPreviousT = cell.mValue;
				row.mCellRow.erase(it1);
				mnSize--;

				if(row.mCellRow.empty())   // If the row is now empty and thus has no more columns...
					mRowMap.erase(it);           // Remove the row from the row map.
				return true;
			}
		}

		return false;
	}


	///////////////////////////////////////////////////////////////////////////////
	// remove_row
	//
	template <typename T, typename Allocator>
	bool sparse_matrix<T, Allocator>::remove_row(int nRow, size_type nCount)
	{
		EASTL_ASSERT((nRow < kSparseMatrixIntMax / 2) && (nRow > kSparseMatrixIntMin / 2));

		// Faster map-based technique:
		for(int i(nRow), iEnd(nRow + (int)nCount); i < iEnd; i++)
		{
			typename RowMap::iterator it(mRowMap.find(i));

			if(it != mRowMap.end()) // If the row is present...
			{
				row_type& row = (*it).second;

				mnSize -= row.mCellRow.size();
				mRowMap.erase(it);
			}
		}

		return true;
	}


	///////////////////////////////////////////////////////////////////////////////
	// remove_col
	//
	template <typename T, typename Allocator>
	bool sparse_matrix<T, Allocator>::remove_col(int nCol, size_type nCount)
	{
		EASTL_ASSERT((nCol < kSparseMatrixIntMax / 2) && (nCol > kSparseMatrixIntMin / 2));

		// Faster map-based version:
		for(typename RowMap::iterator it(mRowMap.begin()); it != mRowMap.end(); ) // For each row...
		{
			row_type& row = (*it).second;

			for(int i(nCol), iEnd(nCol + (int)nCount); i < iEnd; i++)
			{
				typename CellMap::iterator it1(row.mCellRow.find(i));

				if(it1 != row.mCellRow.end()) // If the col is present...
				{
					row.mCellRow.erase(it1);
					mnSize--;
				}
			}

			if(row.mCellRow.empty())
				mRowMap.erase(it++);
			else
				++it;
		}

		return true;
	}


	template <typename T, typename Allocator>
	inline bool sparse_matrix<T, Allocator>::clear()
	{
		mRowMap.clear(); // Clear out the map of maps.
		mnSize = 0;
		return true;
	}


	template <typename T, typename Allocator>
	void sparse_matrix<T, Allocator>::insert(int nRow, int nCol, const T& t, T* pPreviousT)
	{
		EASTL_ASSERT((nCol < kSparseMatrixIntMax / 2) && (nCol > kSparseMatrixIntMin / 2));
		EASTL_ASSERT((nRow < kSparseMatrixIntMax / 2) && (nRow > kSparseMatrixIntMin / 2));

		typename RowMap::iterator it(mRowMap.find(nRow));

		if(it != mRowMap.end()) // If the row is already present...
		{
			row_type& row = (*it).second;

			typename CellMap::iterator it1(row.mCellRow.find(nCol));

			if(it1 != row.mCellRow.end()) // If the col is already present...
			{
				cell_type& cell = (*it1).second;

				if(pPreviousT)
					*pPreviousT = cell.mValue;
				cell.mValue = t; 
				// Note that we leave 'mnSize' as is.
			}
			else
			{
				const typename CellMap::value_type insertionPair(nCol, cell_type(nCol, t));
				row.mCellRow.insert(insertionPair);
				mnSize++;
			}
		}
		else // Else the row doesn't exist (and the column in that row doesn't exist either).
		{
			const typename RowMap::value_type insertionPair(nRow, row_type(nRow));

			eastl::pair<typename RowMap::iterator, bool> insertionResult = mRowMap.insert(insertionPair);
			row_type& row = (*insertionResult.first).second;
		
			EASTL_ASSERT(row.mRow == nRow);         // Make sure we are now on the row we just inserted.
			const typename CellMap::value_type insertionPair1(nCol, cell_type(nCol, t));
			row.mCellRow.insert(insertionPair1);    // Now add the new cell to the new row.
			mnSize++;
		}
	}


	template <typename T, typename Allocator>
	bool sparse_matrix<T, Allocator>::IsCellUsed(int nRow, int nCol)
	{
		EASTL_ASSERT((nCol < kSparseMatrixIntMax / 2) && (nCol > kSparseMatrixIntMin / 2));
		EASTL_ASSERT((nRow < kSparseMatrixIntMax / 2) && (nRow > kSparseMatrixIntMin / 2));

		typename RowMap::iterator it(mRowMap.find(nRow));

		if(it != mRowMap.end())
		{
			row_type& row = (*it).second;

			typename CellMap::iterator it1(row.mCellRow.find(nCol));
			if(it1 != row.mCellRow.end())
				return true;
		}

		return false;
	}


	template <typename T, typename Allocator>
	bool sparse_matrix<T, Allocator>::GetCell(int nRow, int nCol, T* pT)
	{
		EASTL_ASSERT((nCol < kSparseMatrixIntMax / 2) && (nCol > kSparseMatrixIntMin / 2));

		row_type*  pRow;
		cell_type* pCell;

		if(GetMatrixRow(nRow, pRow))
		{
			if(pRow->GetMatrixCol(nCol, pCell))
			{
				if(pT)
					*pT = pCell->mValue;
				return true;
			}
		}

		return false;
	}


	template <typename T, typename Allocator>
	bool sparse_matrix<T, Allocator>::GetCellPtr(int nRow, int nCol, T** pT)
	{
		EASTL_ASSERT((nCol < kSparseMatrixIntMax / 2) && (nCol > kSparseMatrixIntMin / 2));

		row_type*  pRow;
		cell_type* pCell;

		if(GetMatrixRow(nRow, pRow))
		{
			if(pRow->GetMatrixCol(nCol, pCell))
			{
				if(pT)
					*pT = &pCell->mValue;
				return true;
			}
		}

		return false;
	}


	template <typename T, typename Allocator>
	bool sparse_matrix<T, Allocator>::GetMatrixRow(int nRow, row_type*& pRow)
	{
		EASTL_ASSERT((nRow < kSparseMatrixIntMax / 2) && (nRow > kSparseMatrixIntMin / 2));

		typename RowMap::iterator it(mRowMap.find(nRow));

		if(it != mRowMap.end())
		{
			row_type& row = (*it).second;
			pRow = &row;
			return true;
		}

		return false;
	}


	///////////////////////////////////////////////////////////////////////////////
	// FindCell
	//
	// Searches all cells for a match for input data 't'. Writes the cell data into
	// the user celldata array. Call with a NULL pCellArray to get the count.
	//
	// This is a simple search function. Many real-world applications would need a 
	// slightly more flexible search function or mechanism.
	//
	template <typename T, typename Allocator>
	int sparse_matrix<T, Allocator>::FindCell(const T& t, UserCell* pCellArray)
	{
		int nCount(0);

		for(typename RowMap::iterator it(mRowMap.begin()); it != mRowMap.end(); ++it)
		{
			row_type& row = (*it).second;

			for(typename CellMap::iterator it1(row.mCellRow.begin()); it1 != row.mCellRow.end(); ++it1)
			{
				cell_type& cell = (*it1).second;

				if(cell.mValue == t)
				{
					if(pCellArray)
					{
						UserCell& cell = pCellArray[nCount];

						cell.mCol = cell.mCol;
						cell.mRow = row.mRow;
						cell.mValue   = t;
					}
					nCount++; 
				}
			}
		}

		return nCount;
	}


	///////////////////////////////////////////////////////////////////////////////
	// validate
	//
	template <typename T, typename Allocator>
	bool sparse_matrix<T, Allocator>::validate()
	{
		int         nPreviousCol;
		int         nPreviousRow = kRowColIndexNone;
		size_type   nActualTotalCells = 0;

		for(typename RowMap::iterator it(mRowMap.begin()); it != mRowMap.end(); ++it)
		{
			row_type& row = (*it).second;

			if(row.mCellRow.empty())
			{
				// EASTL_TRACE("sparse_matrix::validate(): Error: Empty Cell Row %d.\n", row.mRow);
				return false;
			}

			nPreviousCol = kRowColIndexNone;

			for(typename CellMap::iterator it1(row.mCellRow.begin()); it1 != row.mCellRow.end(); ++it1)
			{
				cell_type& cell = (*it1).second;

				if(cell.mCol <= nPreviousCol)
				{
					// EASTL_TRACE("sparse_matrix::validate(): Error: Columns out of order in row, col: %d, %d.\n", row.mRow, cell.mCol);
					return false;
				}

				nPreviousCol = cell.mCol;
				nActualTotalCells++;
			}

			if(row.mRow <= nPreviousRow)
			{
				// EASTL_TRACE("sparse_matrix::validate(): Error: Rows out of order at row: %d.\n", row.mRow);
				return false;
			}

			nPreviousRow = row.mRow;
		}

		if(mnSize != nActualTotalCells)
		{
			// EASTL_TRACE("sparse_matrix::validate(): Error: 'mnSize' != counted cells %d != %d\n", mnSize, nActualTotalCells);
			return false;
		}

		return true;
	}


	template <typename T, typename Allocator>
	int sparse_matrix<T, Allocator>::validate_iterator(const_iterator i) const
	{
		// To do: Complete this. The value below is a potential false positive.
		return (isf_valid | isf_current | isf_can_dereference);
	}




	///////////////////////////////////////////////////////////////////////////
	// global operators
	///////////////////////////////////////////////////////////////////////////

	template <typename T, typename Allocator>
	bool operator==(sparse_matrix<T, Allocator>& a, sparse_matrix<T, Allocator>& b)
	{
		return (a.mRowMap == b.mRowMap);
	}

	template <typename T, typename Allocator>
	bool operator<(sparse_matrix<T, Allocator>& a, sparse_matrix<T, Allocator>& b)
	{
		return (a.mRowMap < b.mRowMap);
	}

	template <typename T, typename Allocator>
	bool operator!=(sparse_matrix<T, Allocator>& a, sparse_matrix<T, Allocator>& b)
	{
		return !(a.mRowMap == b.mRowMap);
	}

	template <typename T, typename Allocator>
	bool operator>(sparse_matrix<T, Allocator>& a, sparse_matrix<T, Allocator>& b)
	{
		return (b.mRowMap < a.mRowMap);
	}

	template <typename T, typename Allocator>
	bool operator<=(sparse_matrix<T, Allocator>& a, sparse_matrix<T, Allocator>& b)
	{
		return !(b.mRowMap < a.mRowMap);
	}

	template <typename T, typename Allocator>
	bool operator>=(sparse_matrix<T, Allocator>& a, sparse_matrix<T, Allocator>& b)
	{
		return !(a.mRowMap < b.mRowMap);
	}

	template <typename T, typename Allocator>
	void swap(sparse_matrix<T, Allocator>& a, sparse_matrix<T, Allocator>& b)
	{
		a.swap(b);
	}



} // namespace eastl

#endif

#endif // Header include guard















