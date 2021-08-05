#ifndef MATRIX_H
#define MATRIX_H

template <class T> class Matrix;
template<class T> class Row
{
	friend class Matrix<T>;
public:
	Row() = delete;
	T& operator[](size_t j);
	const T& operator[](size_t j) const;

private:
	explicit Row(T* inData, size_t inSize);
	
	T* data;
	size_t size;
};

template<class T> class Matrix final
{
public:
	Matrix();
	Matrix(size_t inRowsCount, size_t inColumnsCount);
	explicit Matrix(size_t inSize);
	Matrix(std::initializer_list<std::initializer_list<T> > inInitializerList);
	Matrix(const Matrix& inOther) = default;
	Matrix(Matrix&& inOther) noexcept = default;
	Matrix& operator=(const Matrix& inOther) = default;
	Matrix& operator=(Matrix&& inOther) noexcept = default;
	~Matrix() = default;

	Row<T> operator[](size_t i);
	Row<T> operator[](size_t i) const;
	[[nodiscard]] size_t getRowsCount() const;
	[[nodiscard]] size_t getColumnsCount() const;

#ifndef NDEBUG
	void print() const;
#endif
	void reset();
	[[nodiscard]] std::shared_ptr<Matrix> operator*(const Matrix& inOtherMatrix) const;
	[[nodiscard]] std::shared_ptr<Matrix> transpose();

private:
	std::shared_ptr<T**> matrix;
	size_t rowsCount;
	size_t columnsCount;
};

template <class T> T& Row<T>::operator[](size_t j)
{
	assert(j < size);
	return data[j];
}

template <class T> const T& Row<T>::operator[](size_t j) const
{
	assert(j < size);
	return data[j];
}

template <class T> Row<T>::Row(T* inData, const size_t inSize) : data(inData), size(inSize) 
{
}

template <class T> Matrix<T>::Matrix() : matrix(nullptr), rowsCount(0), columnsCount(0)
{
}

template <class T> Matrix<T>::Matrix(const size_t inRowsCount, const size_t inColumnsCount) : matrix(std::make_shared<T**>(new T*[inRowsCount])), rowsCount(inRowsCount), columnsCount(inColumnsCount)
{
	for(size_t i = 0; i < rowsCount; ++i)
	{
		(*matrix)[i] = new T[columnsCount];
		for(size_t j = 0; j < columnsCount; ++j)
		{
			(*matrix)[i][j] = 0;
		}
	}
}

template <class T> Matrix<T>::Matrix(const size_t inSize) : Matrix(inSize, inSize)  // NOLINT(cppcoreguidelines-pro-type-member-init)
{
}

template <class T> Matrix<T>::Matrix(std::initializer_list<std::initializer_list<T>> inInitializerList) : matrix(std::make_shared<T**>(new T*[inInitializerList.size()])), rowsCount(inInitializerList.size()), columnsCount(inInitializerList.begin()->size()) //TODO ensure nested type has constant size at compile time
{
	size_t i = 0;
	for(const auto& nestedInitializerList : inInitializerList)
	{
		assert(nestedInitializerList.size() == columnsCount); //TODO remove this when checked at compile time
		(*matrix)[i] = new T[columnsCount];
		size_t j = 0;
		for(const auto& initializerListElement : nestedInitializerList)
		{
			(*matrix)[i][j] = initializerListElement;
			++j;
		}
		++i;
	}
}

template <class T> Row<T> Matrix<T>::operator[](const size_t i)
{
	assert(i < rowsCount);
	return	Row<T>((*matrix)[i], columnsCount);
}

template <class T> Row<T> Matrix<T>::operator[](size_t i) const
{
	assert(i < rowsCount);
	return	Row<T>((*matrix)[i], columnsCount);
}

template <class T> size_t Matrix<T>::getRowsCount() const
{
	return rowsCount;
}

template <class T> size_t Matrix<T>::getColumnsCount() const
{
	return columnsCount;
}

#ifndef NDEBUG
template <class T> void Matrix<T>::print() const
{
	for(size_t i = 0; i < rowsCount; ++i)    
	{    
		for(size_t j = 0; j < columnsCount; ++j)    
		{    
			PRINT((*matrix)[i][j]);
			if(j != columnsCount - 1)
			{
				PRINT(",");
			}
		}
		PRINTLN("");
	}
}
#endif

template <class T> void Matrix<T>::reset()
{
	for(size_t i = 0; i < rowsCount; ++i)    
	{    
		for(size_t j = 0; j < rowsCount; ++j)    
		{    
			matrix[i][j] = 0;      
		}    
	}
}

template <class T> std::shared_ptr<Matrix<T> > Matrix<T>::operator*(const Matrix& inOtherMatrix) const
{
	assert(columnsCount == inOtherMatrix.rowsCount);
	auto result = std::make_shared<Matrix<T> >(rowsCount, columnsCount);
	for(size_t i = 0; i < rowsCount; ++i)    
	{    
		for(size_t j = 0; j < inOtherMatrix.columnsCount; ++j)    
		{    
			(*result)[i][j] = 0;    
			for(size_t k = 0; k < columnsCount; ++k)    
			{    
				(*result)[i][j] += (*matrix)[i][k] * (*inOtherMatrix.matrix)[k][j];    
			}    
		}    
	}
	return result;
}

template <class T> std::shared_ptr<Matrix<T>> Matrix<T>::transpose()
{
	auto result = std::make_shared<Matrix<T> >(columnsCount, rowsCount);
    for (size_t i = 0; i < rowsCount; ++i)
    {
        for (size_t j = 0; j < columnsCount; ++j)
        {
            (*result->matrix)[j][i] = (*matrix)[i][j];
		}
	}
	return result;
}
#endif // MATRIX_H
