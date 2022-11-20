#ifndef TBFRANDOM_HPP
#define TBFRANDOM_HPP

#include <array>
#include <random>

/// @brief Generator of random particle positions within a box. Ctor initialises the generator, and the object generates a new random element, i.e., an array of coordinates
/// @tparam RealType_T Simulation precisio, presumabely float or double
/// @tparam NbValuesPerItem_T Number of coordinates, i.e., problem spacial dimension, 1,2, or 3
template <class RealType_T, long int NbValuesPerItem_T>
class TbfRandom{
public:
    using RealType = RealType_T;
    constexpr static long int NbValuesPerItem = NbValuesPerItem_T;

private:
    std::mt19937 rng;
    std::array<std::uniform_real_distribution<RealType>, NbValuesPerItem> dist;

public:
    using UsedRealType = RealType;

    template <class LimitsContainer>
    TbfRandom(const LimitsContainer& inMaxExc)
        : rng(0) {
        for(long int idxVal = 0 ; idxVal < NbValuesPerItem ; ++idxVal){
            dist[idxVal] = std::uniform_real_distribution<RealType>(0, inMaxExc[idxVal]);
        }
    }

    explicit TbfRandom() : TbfRandom(0, 1){}

    template <class ItemContainer>
    void fillNewItem(ItemContainer* const inOutContainer){
        for(long int idxVal = 0 ; idxVal < NbValuesPerItem ; ++idxVal){
            (*inOutContainer)[idxVal] = dist[idxVal](rng);
        }
    }

    std::array<RealType, NbValuesPerItem> getNewItem(){
        std::array<RealType, NbValuesPerItem> item;
        fillNewItem(&item);
        return item;
    }
};

#endif
