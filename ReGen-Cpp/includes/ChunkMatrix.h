#ifndef CHUNK_MATRIX_H
#define CHUNK_MATRIX_H

#include <map>
#include <array>

template<class ValueType, int RegionSize = 16> class ChunkMatrix
{
	static constexpr size_t REGION_MINOR = RegionSize - 1;
	static constexpr size_t REGION_MAJOR = ~REGION_MINOR;
	typedef std::array<std::array<ValueType, RegionSize>, RegionSize> Chunk;

public:
    ChunkMatrix() = default;
	ValueType& getValueAt(int inX, int inY);
    const ValueType& getValueAt(int inX, int inY) const;
	void setValueAt(ValueType inNode, int inX, int inY);

private:
    mutable std::map<std::pair<int, int>, Chunk> world;
};

template <class ValueType, int RegionSize> const ValueType& ChunkMatrix<ValueType, RegionSize>::getValueAt(const int inX, const int inY) const
{
	return const_cast<ChunkMatrix<ValueType, RegionSize>*>(this)->getValueAt(inX, inY);
}

template <class ValueType, int RegionSize> void ChunkMatrix<ValueType, RegionSize>::setValueAt(ValueType inNode, const int inX, const int inY)
{
	getValueAt(inX, inY) = std::move(inNode);
}

template <class ValueType, int RegionSize> ValueType& ChunkMatrix<ValueType, RegionSize>::getValueAt(const int inX, const int inY)
{
	//Note: operator[] auto instantiate if nothing is found
    auto &chunk = world[std::pair<int, int>(static_cast<int>(inX & REGION_MAJOR), static_cast<int>(inY & REGION_MAJOR))];
    auto &node = chunk[inX & REGION_MINOR][inY & REGION_MINOR];
    return node;
}

#endif //CHUNK_MATRIX_H
