#ifndef TBFHILBERTSPACEINDEX_HPP
#define TBFHILBERTSPACEINDEX_HPP

#include "tbfglobal.hpp"

#include "utils/tbfutils.hpp"
#include "core/tbfinteraction.hpp"

#include <vector>
#include <array>
#include <cassert>

template <long int Dim_T, class ConfigurationClass_T, const bool IsPeriodic_v = false>
class TbfHilbertSpaceIndex{
public:
    static_assert (Dim_T > 0, "Dimension must be greater than 0" );
    static_assert (Dim_T == 3, "Dimension must be 3 for now" );

    using IndexType = long int;
    using ConfigurationClass = ConfigurationClass_T;
    using RealType = typename ConfigurationClass::RealType;

    static constexpr long int Dim = Dim_T;
    static constexpr bool IsPeriodic = IsPeriodic_v;

protected:
    const ConfigurationClass configuration;

    long int getTreeCoordinate(const RealType inRelativePosition, const long int inDim) const {
        assert(inRelativePosition >= 0 && inRelativePosition <= configuration.getBoxWidths()[inDim]);
        if(inRelativePosition == configuration.getBoxWidths()[inDim]){
            return (1 << (configuration.getTreeHeight()-1))-1;
        }
        const RealType indexFReal = inRelativePosition / configuration.getLeafWidths()[inDim];
        return static_cast<long int>(indexFReal);
    }

    /// The following code has been taken from FMB

    typedef struct {
      unsigned char triplet; /* used for its bits */
      short next_state; /* used as a decimal number */
    } Hilbert_Morton_table_case_t;

    static const int CHILDREN_BITS_NUMBER = 3;

    static constexpr Hilbert_Morton_table_case_t Hilbert2Morton_table[12][8] = {
      { { 0x0, 1}, { 0x1, 2}, {0x3, 2}, { 0x2, 3}, { 0x6, 3}, { 0x7, 5}, { 0x5, 5}, { 0x4, 4}},
      { { 0x0, 2}, { 0x2, 0}, {0x6, 0}, { 0x4, 8}, { 0x5, 8}, { 0x7, 7}, { 0x3, 7}, { 0x1, 6}},
      { { 0x0, 0}, { 0x4, 1}, {0x5, 1}, { 0x1, 9}, { 0x3, 9}, { 0x7, 11}, { 0x6, 11}, { 0x2, 10}},

      { { 0x3, 11}, { 0x2, 6}, { 0x0, 6}, { 0x1, 0}, { 0x5, 0}, { 0x4, 9}, { 0x6, 9}, { 0x7, 8}},
      { { 0x5, 9}, { 0x7, 7}, { 0x3, 7}, { 0x1, 11}, { 0x0, 11}, { 0x2, 0}, { 0x6, 0}, { 0x4, 5}},
      { { 0x6, 10}, { 0x2, 8}, { 0x3, 8}, { 0x7, 6}, { 0x5, 6}, { 0x1, 4}, { 0x0, 4}, { 0x4, 0}},

      { { 0x3, 3}, { 0x7, 11}, { 0x6, 11}, { 0x2, 5}, { 0x0, 5}, { 0x4, 1}, { 0x5, 1}, { 0x1, 7}},
      { { 0x5, 4}, { 0x4, 9}, { 0x6, 9}, { 0x7, 10}, { 0x3, 10}, { 0x2, 6}, { 0x0, 6}, { 0x1, 1}},
      { { 0x6, 5}, { 0x4, 10}, { 0x0, 10}, { 0x2, 1}, { 0x3, 1}, { 0x1, 3}, { 0x5, 3}, { 0x7, 9}},

      { { 0x5, 7}, { 0x1, 4}, { 0x0, 4}, { 0x4, 2}, { 0x6, 2}, { 0x2, 8}, { 0x3, 8}, { 0x7, 3}},
      { { 0x6, 8}, { 0x7, 5}, { 0x5, 5}, { 0x4, 7}, { 0x0, 7}, { 0x1, 2}, { 0x3, 2}, { 0x2, 11}},
      { { 0x3, 6}, { 0x1, 3}, { 0x5, 3}, { 0x7, 4}, { 0x6, 4}, { 0x4, 10}, { 0x0, 10}, { 0x2, 2}},
    };


    static constexpr Hilbert_Morton_table_case_t Morton2Hilbert_table[12][8] = {
      { { 0x0, 1}, { 0x1, 2}, { 0x3, 3}, { 0x2, 2}, { 0x7, 4}, { 0x6, 5}, { 0x4, 3}, { 0x5, 5}},
      { { 0x0, 2}, { 0x7, 6}, { 0x1, 0}, { 0x6, 7}, { 0x3, 8}, { 0x4, 8}, { 0x2, 0}, { 0x5, 7}},
      { { 0x0, 0}, { 0x3, 9}, { 0x7, 10}, { 0x4, 9}, { 0x1, 1}, { 0x2, 1}, { 0x6, 11}, { 0x5, 11}},

      { { 0x2, 6}, { 0x3, 0}, { 0x1, 6}, { 0x0, 11}, { 0x5, 9}, { 0x4, 0}, { 0x6, 9}, { 0x7, 8}},
      { { 0x4, 11}, { 0x3, 11}, { 0x5, 0}, { 0x2, 7}, { 0x7, 5}, { 0x0, 9}, { 0x6, 0}, { 0x1, 7}},
      { { 0x6, 4}, { 0x5, 4}, { 0x1, 8}, { 0x2, 8}, { 0x7, 0}, { 0x4, 6}, { 0x0, 10}, { 0x3, 6}},

      { { 0x4, 5}, { 0x7, 7}, { 0x3, 5}, { 0x0, 3}, { 0x5, 1}, { 0x6, 1}, { 0x2, 11}, { 0x1, 11}},
      { { 0x6, 6}, { 0x7, 1}, { 0x5, 6}, { 0x4, 10}, { 0x1, 9}, { 0x0, 4}, { 0x2, 9}, { 0x3, 10}},
      { { 0x2, 10}, { 0x5, 3}, { 0x3, 1}, { 0x4, 1}, { 0x1, 10}, { 0x6, 3}, { 0x0, 5}, { 0x7, 9}},

      { { 0x2, 4}, { 0x1, 4}, { 0x5, 8}, { 0x6, 8}, { 0x3, 2}, { 0x0, 7}, { 0x4, 2}, { 0x7, 3}},
      { { 0x4, 7}, { 0x5, 2}, { 0x7, 11}, { 0x6, 2}, { 0x3, 7}, { 0x2, 5}, { 0x0, 8}, { 0x1, 5}},
      { { 0x6, 10}, { 0x1, 3}, { 0x7, 2}, { 0x0, 6}, { 0x5, 10}, { 0x2, 3}, { 0x4, 4}, { 0x3, 4}},
    };


    long int Hilbert2Morton(long int Hilbert_ind) const{
      long int mask = 0x7LL;
      long int h;
      long int shift = 0;
      long int current_state = 0;
      long int Hilbert_triplet;
      long int Morton_triplet; /* can NOT be defined as an "unsigned char"! */
      long int res = 0x0LL;

      shift = (configuration.getTreeHeight()-1) * CHILDREN_BITS_NUMBER;
      mask <<= shift; /* Set the mask on the triplet of bits corresponding to the first level. */

      for (h=configuration.getTreeHeight(); h>0; --h){
        Hilbert_triplet = (Hilbert_ind & mask) >> shift; /* Get the triplet of bits corresponding
                                                          * to the current level. */

        /* Get the corresponding triplet with Morton ordering: */
        Morton_triplet = Hilbert2Morton_table[current_state][Hilbert_triplet].triplet;
        /* Get state for next iteration: */
        current_state = Hilbert2Morton_table[current_state][Hilbert_triplet].next_state;

        /* Write Morton triplet at the place according to the current level: */
        res |= (Morton_triplet << shift);

        mask >>= CHILDREN_BITS_NUMBER;
        shift -= CHILDREN_BITS_NUMBER;
      }

      return res;
    }


    long int Morton2Hilbert(long int Morton_ind) const {
      long int mask = 0x7LL;
      long int h;
      long int shift = 0;
      long int current_state = 0;
      long int  Hilbert_triplet;  /* can NOT be defined as an "unsigned char"! */
      long int Morton_triplet;
      long int res = 0x0LL;

      shift = (configuration.getTreeHeight()-1) * CHILDREN_BITS_NUMBER;
      mask <<= shift; /* Set the mask on the triplet of bits corresponding to the first level. */

      for (h=configuration.getTreeHeight(); h>0; --h){
        Morton_triplet = (Morton_ind & mask) >> shift; /* Get the triplet of bits corresponding
                                                        * to the current level. */

        /* Get the corresponding triplet with Morton ordering: */
        Hilbert_triplet = Morton2Hilbert_table[current_state][Morton_triplet].triplet;
        /* Get state for next iteration: */
        current_state = Morton2Hilbert_table[current_state][Morton_triplet].next_state;

        /* Write Morton triplet at the place according to the current level: */
        res |= (Hilbert_triplet << shift);

        mask >>= CHILDREN_BITS_NUMBER;
        shift -= CHILDREN_BITS_NUMBER;
      }

      return res;
    }

public:
    TbfHilbertSpaceIndex(const ConfigurationClass& inConfiguration)
        : configuration(inConfiguration){
    }

    IndexType getUpperBound(const long int inLevel) const{
        return (IndexType(1) << (inLevel * Dim));
    }

    template <class PositionType>
    IndexType getIndexFromPosition(const PositionType& inPos) const {
        std::array<long int,Dim> host;
        for(long int idxDim = 0 ; idxDim < Dim ; ++idxDim){
            host[idxDim] = getTreeCoordinate( inPos[idxDim] - configuration.getBoxCorner()[idxDim], idxDim);
        }

        return Morton2Hilbert(getIndexFromBoxPos(host));
    }

    std::array<RealType,Dim> getRealPosFromBoxPos(const std::array<long int,Dim>& inPos) const {
        const std::array<RealType,Dim> boxCorner(configuration.getBoxCenter(),-(configuration.getBoxWidths()/2));

        std::array<RealType,Dim> host;

        for(long int idxDim = 0 ; idxDim < Dim ; ++idxDim){
            host[idxDim] = ( inPos[idxDim]*configuration.getLeafWidths()[idxDim] + boxCorner[idxDim] );
        }

        return host;
    }

    std::array<long int,Dim> getBoxPosFromIndex(IndexType inMindexHilbert) const{
        IndexType inMindex = Hilbert2Morton(inMindexHilbert);
        IndexType mask = 0x1LL;

        std::array<long int,Dim> boxPos;

        for(long int idxDim = 0 ; idxDim < Dim ; ++idxDim){
            boxPos[idxDim] = 0;
        }

        while(inMindex >= mask) {
            for(long int idxDim = Dim-1 ; idxDim > 0 ; --idxDim){
                boxPos[idxDim] |= static_cast<long int>(inMindex & mask);
                inMindex >>= 1;
            }

            boxPos[0] |= static_cast<long int>(inMindex & mask);

            mask <<= 1;
        }

        return boxPos;
    }

    IndexType getParentIndex(IndexType inIndex) const{
        return inIndex >> Dim;
    }

    long int childPositionFromParent(const IndexType inIndexChild) const {
        return inIndexChild & static_cast<long int>(~(((~0UL)>>Dim)<<Dim));
    }

    IndexType getIndexFromBoxPos(const std::array<long int,Dim>& inBoxPos) const{
        IndexType index = 0x0LL;
        IndexType mask = 0x1LL;

        bool shouldContinue = false;

        std::array<IndexType,Dim> mcoord;
        for(long int idxDim = 0 ; idxDim < Dim ; ++idxDim){
            mcoord[idxDim] = (inBoxPos[idxDim] << (Dim - idxDim - 1));
            shouldContinue |= ((mask << (Dim - idxDim - 1)) <= mcoord[idxDim]);
        }

        while(shouldContinue){
            shouldContinue = false;
            for(long int idxDim = Dim-1 ; idxDim >= 0 ; --idxDim){
                index |= (mcoord[idxDim] & mask);
                mask <<= 1;
                mcoord[idxDim] <<= (Dim-1);
                shouldContinue |= ((mask << (Dim - idxDim - 1)) <= mcoord[idxDim]);
            }
        }

        return Morton2Hilbert(index);
    }

    IndexType getChildIndexFromParent(const IndexType inParentIndex, const long int inChild) const{
        return (inParentIndex<<Dim) + inChild;
    }

    auto getInteractionListForIndex(const IndexType inMIndex, const long int inLevel) const{
        std::vector<IndexType> indexes;

        if constexpr(IsPeriodic == false){
            if(inLevel < 2){
                return indexes;
            }
        }
        else{
            if(inLevel < 1){
                return indexes;
            }
        }

        const long int boxLimite = (1 << (inLevel));
        const long int boxLimiteParent = (1 << (inLevel-1));

        const IndexType cellIndex = inMIndex;
        const auto cellPos = getBoxPosFromIndex(cellIndex);

        const IndexType parentCellIndex = getParentIndex(cellIndex);
        const auto parentCellPos = getBoxPosFromIndex(parentCellIndex);


        std::array<long int, Dim> minLimits;
        std::array<long int, Dim> maxLimits;
        std::array<long int, Dim> currentParentTest;

        for(long int idxDim = 0 ; idxDim < Dim ; ++idxDim){
            if constexpr(IsPeriodic == false){
                if(parentCellPos[idxDim] == 0){
                    minLimits[idxDim] = 0;
                }
                else{
                    minLimits[idxDim] = -1;
                }
                if(parentCellPos[idxDim]+1 == boxLimiteParent){
                    maxLimits[idxDim] = 0;
                }
                else{
                    maxLimits[idxDim] = 1;
                }
            }
            else{
                minLimits[idxDim] = -1;
                maxLimits[idxDim] = 1;
            }
            currentParentTest[idxDim] = minLimits[idxDim];
        }


        while(true){
            {
                long int currentIdx = Dim-1;

                while(currentIdx >= 0 && currentParentTest[currentIdx] > maxLimits[currentIdx]){
                    currentParentTest[currentIdx] = minLimits[currentIdx];
                    currentIdx -= 1;
                    if(currentIdx >= 0){
                        currentParentTest[currentIdx] += 1;
                    }
                }
                if(currentIdx < 0){
                    break;
                }
            }

            auto otherParentPos = TbfUtils::AddVecToVec(parentCellPos, currentParentTest);
            auto periodicShift = TbfUtils::make_array<long int, Dim>(0);

            if constexpr(IsPeriodic){
                for(long int idxDim = 0 ; idxDim < Dim ; ++idxDim){
                    if(otherParentPos[idxDim] < 0){
                        periodicShift[idxDim] = -boxLimite;
                        otherParentPos[idxDim] += boxLimiteParent;
                    }
                    else if(boxLimite <= otherParentPos[idxDim]){
                        periodicShift[idxDim] = boxLimite;
                        otherParentPos[idxDim] -= boxLimiteParent;
                    }
                }
            }
            const IndexType otherParentIndex = getIndexFromBoxPos(otherParentPos);

            for(long int idxChild = 0 ; idxChild < (1<<Dim) ; ++idxChild){
                const IndexType childIndex = getChildIndexFromParent(otherParentIndex, idxChild);
                auto childPos = getBoxPosFromIndex(childIndex);

                bool isTooClose = true;
                for(int idxDim = 0 ; isTooClose && idxDim < Dim ; ++idxDim){
                    if(std::abs(childPos[idxDim] + periodicShift[idxDim] - cellPos[idxDim]) > 1){
                        isTooClose = false;
                    }
                }

                if(isTooClose == false){
                    long int arrayPos = 0;
                    for(int idxDim = 0 ; idxDim < Dim ; ++idxDim){
                        arrayPos *= 7;
                        arrayPos += (childPos[idxDim] + periodicShift[idxDim] - cellPos[idxDim] + 3);
                    }
                    assert(arrayPos < TbfUtils::lipow(7,Dim));

                    indexes.push_back(childIndex);
                }
            }

            currentParentTest[Dim-1] += 1;
        }

        return indexes;
    }

    template <class GroupClass>
    auto getInteractionListForBlock(const GroupClass& inGroup, const long int inLevel, const bool testSelfInclusion = true) const{
        assert(inLevel >= 0);

        std::vector<TbfXtoXInteraction<IndexType>> indexesInternal;
        indexesInternal.reserve(inGroup.getNbCells());

        std::vector<TbfXtoXInteraction<IndexType>> indexesExternal;
        indexesExternal.reserve(inGroup.getNbCells());

        if constexpr(IsPeriodic == false){
            if(inLevel < 2){
                return std::make_pair(std::move(indexesInternal), std::move(indexesExternal));
            }
        }
        else{
            if(inLevel < 1){
                return std::make_pair(std::move(indexesInternal), std::move(indexesExternal));
            }
        }

        const long int boxLimite = (1 << (inLevel));
        const long int boxLimiteParent = (1 << (inLevel-1));

        for(long int idxCell = 0 ; idxCell < inGroup.getNbCells() ; ++idxCell){
            const IndexType cellIndex = inGroup.getCellSpacialIndex(idxCell);
            const auto cellPos = getBoxPosFromIndex(cellIndex);

            const IndexType parentCellIndex = getParentIndex(cellIndex);
            const auto parentCellPos = getBoxPosFromIndex(parentCellIndex);


            std::array<long int, Dim> minLimits;
            std::array<long int, Dim> maxLimits;
            std::array<long int, Dim> currentParentTest;

            for(long int idxDim = 0 ; idxDim < Dim ; ++idxDim){
                if constexpr(IsPeriodic == false){
                    if(parentCellPos[idxDim] == 0){
                        minLimits[idxDim] = 0;
                    }
                    else{
                        minLimits[idxDim] = -1;
                    }
                    if(parentCellPos[idxDim]+1 == boxLimiteParent){
                        maxLimits[idxDim] = 0;
                    }
                    else{
                        maxLimits[idxDim] = 1;
                    }
                }
                else{
                    minLimits[idxDim] = -1;
                    maxLimits[idxDim] = 1;
                }
                currentParentTest[idxDim] = minLimits[idxDim];
            }


            while(true){
                {
                    long int currentIdx = Dim-1;

                    while(currentIdx >= 0 && currentParentTest[currentIdx] > maxLimits[currentIdx]){
                        currentParentTest[currentIdx] = minLimits[currentIdx];
                        currentIdx -= 1;
                        if(currentIdx >= 0){
                            currentParentTest[currentIdx] += 1;
                        }
                    }
                    if(currentIdx < 0){
                        break;
                    }
                }

                auto otherParentPos = TbfUtils::AddVecToVec(parentCellPos, currentParentTest);
                auto periodicShift = TbfUtils::make_array<long int, Dim>(0);

                if constexpr(IsPeriodic){
                    for(long int idxDim = 0 ; idxDim < Dim ; ++idxDim){
                        if(otherParentPos[idxDim] < 0){
                            periodicShift[idxDim] = -boxLimite;
                            otherParentPos[idxDim] += boxLimiteParent;
                        }
                        else if(boxLimite <= otherParentPos[idxDim]){
                            periodicShift[idxDim] = boxLimite;
                            otherParentPos[idxDim] -= boxLimiteParent;
                        }
                    }
                }
                const IndexType otherParentIndex = getIndexFromBoxPos(otherParentPos);


                for(long int idxChild = 0 ; idxChild < (1<<Dim) ; ++idxChild){
                    const IndexType childIndex = getChildIndexFromParent(otherParentIndex, idxChild);
                    auto childPos = getBoxPosFromIndex(childIndex);

                    bool isTooClose = true;
                    for(int idxDim = 0 ; isTooClose && idxDim < Dim ; ++idxDim){
                        if(std::abs(childPos[idxDim] + periodicShift[idxDim] - cellPos[idxDim]) > 1){
                            isTooClose = false;
                        }
                    }

                    if(isTooClose == false){
                        long int arrayPos = 0;
                        for(int idxDim = 0 ; idxDim < Dim ; ++idxDim){
                            arrayPos *= 7;
                            arrayPos += (childPos[idxDim] + periodicShift[idxDim] - cellPos[idxDim] + 3);
                        }
                        assert(arrayPos < TbfUtils::lipow(7,Dim));

                        TbfXtoXInteraction<IndexType> interaction;
                        interaction.indexTarget = cellIndex;
                        interaction.indexSrc = childIndex;
                        interaction.globalTargetPos = idxCell;
                        interaction.arrayIndexSrc = arrayPos;

                        if(inGroup.getStartingSpacialIndex() <= interaction.indexSrc
                                && interaction.indexSrc <= inGroup.getEndingSpacialIndex()){
                            if(testSelfInclusion == false || inGroup.getElementFromSpacialIndex(interaction.indexSrc)){
                                indexesInternal.push_back(interaction);
                            }
                        }
                        else{
                            indexesExternal.push_back(interaction);
                        }
                    }
                }

                currentParentTest[Dim-1] += 1;
            }
        }

        return std::make_pair(std::move(indexesInternal), std::move(indexesExternal));
    }


    auto getNeighborListForIndex(const IndexType cellIndex, const long int inLevel, const bool upperExclusion = false) const{
        assert(inLevel >= 0);
        const long int boxLimite = (1 << (inLevel));

        std::vector<IndexType> indexes;
        indexes.reserve(TbfUtils::lipow(3,Dim)/2);

        const auto cellPos = getBoxPosFromIndex(cellIndex);

        std::array<long int, Dim> minLimits;
        std::array<long int, Dim> maxLimits;
        std::array<long int, Dim> currentTest;

        for(long int idxDim = 0 ; idxDim < Dim ; ++idxDim){
            if constexpr(IsPeriodic == false){
                if(cellPos[idxDim] == 0){
                    minLimits[idxDim] = 0;
                }
                else{
                    minLimits[idxDim] = -1;
                }
                if(cellPos[idxDim]+1 == boxLimite){
                    maxLimits[idxDim] = 0;
                }
                else{
                    maxLimits[idxDim] = 1;
                }
            }
            else{
                minLimits[idxDim] = -1;
                maxLimits[idxDim] = 1;
            }
            currentTest[idxDim] = minLimits[idxDim];
        }

        while(true){
            {
                long int currentIdx = Dim-1;

                while(currentIdx >= 0 && currentTest[currentIdx] > maxLimits[currentIdx]){
                    currentTest[currentIdx] = minLimits[currentIdx];
                    currentIdx -= 1;
                    if(currentIdx >= 0){
                        currentTest[currentIdx] += 1;
                    }
                }
                if(currentIdx < 0){
                    break;
                }
            }

            bool isSelfCell = true;
            for(int idxDim = 0 ; isSelfCell && idxDim < Dim ; ++idxDim){
                if(currentTest[idxDim] != 0){
                    isSelfCell = false;
                }
            }

            if(isSelfCell == false){
                auto otherPos = TbfUtils::AddVecToVec(cellPos, currentTest);

                long int arrayPos = 0;
                for(int idxDim = 0 ; idxDim < Dim ; ++idxDim){
                    arrayPos *= 3;
                    arrayPos += (otherPos[idxDim] - cellPos[idxDim] + 1);
                }
                assert(arrayPos < TbfUtils::lipow(3, Dim));

                if constexpr(IsPeriodic){
                    for(long int idxDim = 0 ; idxDim < Dim ; ++idxDim){
                        otherPos[idxDim] = ((otherPos[idxDim]+boxLimite)%boxLimite);
                    }
                }

                const IndexType otherIndex = getIndexFromBoxPos(otherPos);

                if(upperExclusion == false || otherIndex < cellIndex){
                    indexes.push_back(otherIndex);
                }
            }

            currentTest[Dim-1] += 1;
        }

        return indexes;
    }

    template <class GroupClass>
    auto getNeighborListForBlock(const GroupClass& inGroup, const long int inLevel, const bool upperExclusion = false, const bool testSelfInclusion = true) const{
        assert(inLevel >= 0);
        const long int boxLimite = (1 << (inLevel));

        std::vector<TbfXtoXInteraction<IndexType>> indexesInternal;
        indexesInternal.reserve(inGroup.getNbLeaves());

        std::vector<TbfXtoXInteraction<IndexType>> indexesExternal;
        indexesExternal.reserve(inGroup.getNbLeaves());

        for(long int idxCell = 0 ; idxCell < inGroup.getNbLeaves() ; ++idxCell){
            const IndexType cellIndex = inGroup.getLeafSpacialIndex(idxCell);
            const auto cellPos = getBoxPosFromIndex(cellIndex);

            std::array<long int, Dim> minLimits;
            std::array<long int, Dim> maxLimits;
            std::array<long int, Dim> currentTest;

            for(long int idxDim = 0 ; idxDim < Dim ; ++idxDim){
                if constexpr(IsPeriodic == false){
                    if(cellPos[idxDim] == 0){
                        minLimits[idxDim] = 0;
                    }
                    else{
                        minLimits[idxDim] = -1;
                    }
                    if(cellPos[idxDim]+1 == boxLimite){
                        maxLimits[idxDim] = 0;
                    }
                    else{
                        maxLimits[idxDim] = 1;
                    }
                }
                else{
                    minLimits[idxDim] = -1;
                    maxLimits[idxDim] = 1;
                }
                currentTest[idxDim] = minLimits[idxDim];
            }


            while(true){
                {
                    long int currentIdx = Dim-1;

                    while(currentIdx >= 0 && currentTest[currentIdx] > maxLimits[currentIdx]){
                        currentTest[currentIdx] = minLimits[currentIdx];
                        currentIdx -= 1;
                        if(currentIdx >= 0){
                            currentTest[currentIdx] += 1;
                        }
                    }
                    if(currentIdx < 0){
                        break;
                    }
                }

                bool isSelfCell = true;
                for(int idxDim = 0 ; isSelfCell && idxDim < Dim ; ++idxDim){
                    if(currentTest[idxDim] != 0){
                        isSelfCell = false;
                    }
                }

                if(isSelfCell == false){
                    auto otherPos = TbfUtils::AddVecToVec(cellPos, currentTest);

                    long int arrayPos = 0;
                    for(int idxDim = 0 ; idxDim < Dim ; ++idxDim){
                        arrayPos *= 3;
                        arrayPos += (otherPos[idxDim] - cellPos[idxDim] + 1);
                    }
                    assert(arrayPos < TbfUtils::lipow(3, Dim));

                    if constexpr(IsPeriodic){
                        for(long int idxDim = 0 ; idxDim < Dim ; ++idxDim){
                            otherPos[idxDim] = ((otherPos[idxDim]+boxLimite)%boxLimite);
                        }
                    }

                    const IndexType otherIndex = getIndexFromBoxPos(otherPos);

                    if(upperExclusion == false || otherIndex < cellIndex){
                        TbfXtoXInteraction<IndexType> interaction;
                        interaction.indexTarget = cellIndex;
                        interaction.indexSrc = otherIndex;
                        interaction.globalTargetPos = idxCell;
                        interaction.arrayIndexSrc = arrayPos;

                        if(inGroup.getStartingSpacialIndex() <= interaction.indexSrc
                                && interaction.indexSrc <= inGroup.getEndingSpacialIndex()){
                            if(testSelfInclusion == false || inGroup.getElementFromSpacialIndex(interaction.indexSrc)){
                                indexesInternal.push_back(interaction);
                            }
                        }
                        else{
                            indexesExternal.push_back(interaction);
                        }
                    }
                }

                currentTest[Dim-1] += 1;
            }
        }

        return std::make_pair(std::move(indexesInternal), std::move(indexesExternal));
    }


    template <class GroupClass>
    auto getSelfListForBlock(const GroupClass& inGroup) const{
        std::vector<TbfXtoXInteraction<IndexType>> indexesSelf;
        indexesSelf.reserve(inGroup.getNbLeaves());

        long int arrayPos = 0;
        for(int idxDim = 0 ; idxDim < Dim ; ++idxDim){
            arrayPos *= 3;
            arrayPos += (1);
        }
        assert(arrayPos < TbfUtils::lipow(3, Dim));

        for(long int idxCell = 0 ; idxCell < inGroup.getNbLeaves() ; ++idxCell){
            const IndexType cellIndex = inGroup.getLeafSpacialIndex(idxCell);

            TbfXtoXInteraction<IndexType> interaction;
            interaction.indexTarget = cellIndex;
            interaction.indexSrc = cellIndex;
            interaction.globalTargetPos = idxCell;
            interaction.arrayIndexSrc = arrayPos;

            indexesSelf.emplace_back(interaction);
        }

        return indexesSelf;
    }


    static long int constexpr getNbChildrenPerCell() {
        return 1L << Dim;
    }

    static long int constexpr getNbNeighborsPerCell() {
        long int nbNeighbors = 1;
        long int nbNeighborsTooClose = 1;
        for(long int idxNeigh = 0 ; idxNeigh < Dim ; ++idxNeigh){
            nbNeighbors *= 6;
            nbNeighborsTooClose *= 3;
        }
        return nbNeighbors - nbNeighborsTooClose;
    }
};

#endif
