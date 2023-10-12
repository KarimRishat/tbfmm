#ifndef TBFSMSPECXALGORITHM_HPP
#define TBFSMSPECXALGORITHM_HPP

#include "tbfglobal.hpp"

#include "../sequential/tbfgroupkernelinterface.hpp"
#include "spacial/tbfspacialconfiguration.hpp"
#include "algorithms/tbfalgorithmutils.hpp"
#include "tbfsmstarpucallbacks.hpp"

#include <cassert>
#include <iterator>
#include <list>

#include <starpu.h>

#include "tbfsmstarpuutils.hpp"

template <class RealType_T, class KernelClass_T, class SpaceIndexType_T = TbfDefaultSpaceIndexType<RealType_T>>
class TbfSmStarpuAlgorithm {
public:
    using RealType = RealType_T;
    using KernelClass = KernelClass_T;
    using SpaceIndexType = SpaceIndexType_T;
    using SpacialConfiguration = TbfSpacialConfiguration<RealType, SpaceIndexType::Dim>;

protected:
    using ThisClass = TbfSmStarpuAlgorithm<RealType_T, KernelClass_T, SpaceIndexType_T>;

    using CellHandleContainer = typename TbfStarPUHandleBuilder::CellHandleContainer;
    using ParticleHandleContainer = typename TbfStarPUHandleBuilder::ParticleHandleContainer;

    using VecOfIndexes = std::vector<TbfXtoXInteraction<typename SpaceIndexType::IndexType>>;
    std::list<VecOfIndexes> vecIndexBuffer;

    starpu_codelet p2m_cl;
    starpu_codelet m2m_cl;
    starpu_codelet l2l_cl;
    starpu_codelet l2l_cl_nocommute;
    starpu_codelet l2p_cl;

    starpu_codelet m2l_cl_between_groups;
    starpu_codelet m2l_cl_inside;

    starpu_codelet p2p_cl_oneleaf;
    starpu_codelet p2p_cl_twoleaves;

    friend TbfSmStarpuCallbacks;

    template<class CellContainerClass, class ParticleContainerClass>
    void initCodelet(){
        memset(&p2m_cl, 0, sizeof(p2m_cl));
        p2m_cl.cpu_funcs[0] = &TbfSmStarpuCallbacks::P2MCallback<ThisClass, CellContainerClass, ParticleContainerClass>;
        p2m_cl.where |= STARPU_CPU;
        p2m_cl.nbuffers = 3;
        p2m_cl.modes[0] = STARPU_R;
        p2m_cl.modes[1] = STARPU_R;
        p2m_cl.modes[2] = starpu_data_access_mode(STARPU_RW|STARPU_COMMUTE);
        p2m_cl.name = "p2m_cl";

        memset(&m2m_cl, 0, sizeof(m2m_cl));
        m2m_cl.cpu_funcs[0] = &TbfSmStarpuCallbacks::M2MCallback<ThisClass, CellContainerClass>;
        m2m_cl.where |= STARPU_CPU;
        m2m_cl.nbuffers = 4;
        m2m_cl.modes[0] = STARPU_R;
        m2m_cl.modes[1] = STARPU_R;
        m2m_cl.modes[2] = STARPU_R;
        m2m_cl.modes[3] = starpu_data_access_mode(STARPU_RW|STARPU_COMMUTE);
        m2m_cl.name = "m2m_cl";

        memset(&l2l_cl, 0, sizeof(l2l_cl));
        l2l_cl.cpu_funcs[0] = &TbfSmStarpuCallbacks::L2LCallback<ThisClass, CellContainerClass>;
        l2l_cl.where |= STARPU_CPU;
        l2l_cl.nbuffers = 4;
        l2l_cl.modes[0] = STARPU_R;
        l2l_cl.modes[1] = STARPU_R;
        l2l_cl.modes[2] = STARPU_R;
        l2l_cl.modes[3] = starpu_data_access_mode(STARPU_RW|STARPU_COMMUTE);
        l2l_cl.name = "l2l_cl";

        memset(&l2p_cl, 0, sizeof(l2p_cl));
        l2p_cl.cpu_funcs[0] = &TbfSmStarpuCallbacks::L2PCallback<ThisClass, CellContainerClass, ParticleContainerClass>;
        l2p_cl.where |= STARPU_CPU;
        l2p_cl.nbuffers = 4;
        l2p_cl.modes[0] = STARPU_R;
        l2p_cl.modes[1] = STARPU_R;
        l2p_cl.modes[2] = STARPU_R;
        l2p_cl.modes[3] = starpu_data_access_mode(STARPU_RW|STARPU_COMMUTE);
        l2p_cl.name = "l2p_cl";

        memset(&p2p_cl_oneleaf, 0, sizeof(p2p_cl_oneleaf));
        p2p_cl_oneleaf.cpu_funcs[0] = &TbfSmStarpuCallbacks::P2POneLeafCallback<ThisClass, ParticleContainerClass>;
        p2p_cl_oneleaf.where |= STARPU_CPU;
        p2p_cl_oneleaf.nbuffers = 2;
        p2p_cl_oneleaf.modes[0] = STARPU_R;
        p2p_cl_oneleaf.modes[1] = starpu_data_access_mode(STARPU_RW|STARPU_COMMUTE);
        p2p_cl_oneleaf.name = "p2p_cl_oneleaf";

        memset(&p2p_cl_twoleaves, 0, sizeof(p2p_cl_twoleaves));
        p2p_cl_twoleaves.cpu_funcs[0] = &TbfSmStarpuCallbacks::P2PBetweenLeavesCallback<ThisClass, ParticleContainerClass>;
        p2p_cl_twoleaves.where |= STARPU_CPU;
        p2p_cl_twoleaves.nbuffers = 4;
        p2p_cl_twoleaves.modes[0] = STARPU_R;
        p2p_cl_twoleaves.modes[1] = starpu_data_access_mode(STARPU_RW|STARPU_COMMUTE);
        p2p_cl_twoleaves.modes[2] = STARPU_R;
        p2p_cl_twoleaves.modes[3] = starpu_data_access_mode(STARPU_RW|STARPU_COMMUTE);
        p2p_cl_twoleaves.name = "p2p_cl_twoleaves";

        memset(&m2l_cl_between_groups, 0, sizeof(m2l_cl_between_groups));
        m2l_cl_between_groups.cpu_funcs[0] = &TbfSmStarpuCallbacks::M2LCallback<ThisClass, CellContainerClass>;
        m2l_cl_between_groups.where |= STARPU_CPU;
        m2l_cl_between_groups.nbuffers = 4;
        m2l_cl_between_groups.modes[0] = STARPU_R;
        m2l_cl_between_groups.modes[1] = STARPU_R;
        m2l_cl_between_groups.modes[2] = STARPU_R;
        m2l_cl_between_groups.modes[3] = starpu_data_access_mode(STARPU_RW|STARPU_COMMUTE);
        m2l_cl_between_groups.name = "m2l_cl_between_groups";

        memset(&m2l_cl_inside, 0, sizeof(m2l_cl_inside));
        m2l_cl_inside.cpu_funcs[0] = &TbfSmStarpuCallbacks::M2LInnerCallback<ThisClass, CellContainerClass>;
        m2l_cl_inside.where |= STARPU_CPU;
        m2l_cl_inside.nbuffers = 3;
        m2l_cl_inside.modes[0] = STARPU_R;
        m2l_cl_inside.modes[1] = STARPU_R;
        m2l_cl_inside.modes[2] = starpu_data_access_mode(STARPU_RW|STARPU_COMMUTE);
        m2l_cl_inside.name = "m2l_cl_inside";
    }

    const SpacialConfiguration configuration;
    const SpaceIndexType spaceSystem;

    const long int stopUpperLevel;

    TbfGroupKernelInterface<SpaceIndexType> kernelWrapper;
    std::vector<KernelClass> kernels;

    TbfAlgorithmUtils::TbfOperationsPriorities priorities;

    template <class TreeClass>
    void P2M(TreeClass& inTree, CellHandleContainer& cellHandles, ParticleHandleContainer& particleHandles){
        if(configuration.getTreeHeight() > stopUpperLevel){
            auto& leafGroups = inTree.getLeafGroups();
            const auto& particleGroups = inTree.getParticleGroups();

            assert(std::size(leafGroups) == std::size(particleGroups));

            auto currentLeafGroup = leafGroups.begin();
            auto currentParticleGroup = particleGroups.cbegin();

            const auto endLeafGroup = leafGroups.end();
            const auto endParticleGroup = particleGroups.cend();
            int idxGroup = 0;

            while(currentLeafGroup != endLeafGroup && currentParticleGroup != endParticleGroup){
                assert((*currentParticleGroup).getStartingSpacialIndex() == (*currentLeafGroup).getStartingSpacialIndex()
                       && (*currentParticleGroup).getEndingSpacialIndex() == (*currentLeafGroup).getEndingSpacialIndex()
                       && (*currentParticleGroup).getNbLeaves() == (*currentLeafGroup).getNbCells());
                auto* thisptr = this;
                unsigned char* groupCellsData = inTree.getLeafGroups()[idxGroup].getDataPtr();
                size_t groupCellsDataSize = inTree.getLeafGroups()[idxGroup].getDataSize();
                unsigned char* groupParticlesData = inTree.getParticleGroups()[idxGroup].getDataPtr();
                size_t groupParticlesDataSize = inTree.getParticleGroups()[idxGroup].getDataSize();
                starpu_insert_task(&p2m_cl,
                                   STARPU_VALUE, &thisptr, sizeof(void*),
                                   STARPU_VALUE, &groupCellsData, sizeof(void*),
                                   STARPU_VALUE, &groupCellsDataSize, sizeof(size_t),
                                   STARPU_VALUE, &groupParticlesData, sizeof(void*),
                                   STARPU_VALUE, &groupParticlesDataSize, sizeof(size_t),
                                   STARPU_PRIORITY, priorities.getP2MPriority(),
                                   STARPU_R, particleHandles[idxGroup][0],
                                   STARPU_R, cellHandles[configuration.getTreeHeight()-1][idxGroup][0],
                                   starpu_data_access_mode(STARPU_RW|STARPU_COMMUTE), cellHandles[configuration.getTreeHeight()-1][idxGroup][1],
                                   STARPU_NAME, "P2M",
                                   0);

                ++currentParticleGroup;
                ++currentLeafGroup;
                ++idxGroup;
            }
        }
    }

    template <class TreeClass>
    void M2M(TreeClass& inTree, CellHandleContainer& cellHandles){
        for(long int idxLevel = configuration.getTreeHeight()-2 ; idxLevel >= stopUpperLevel ; --idxLevel){
            auto& upperCellGroup = inTree.getCellGroupsAtLevel(idxLevel);
            const auto& lowerCellGroup = inTree.getCellGroupsAtLevel(idxLevel+1);

            auto currentUpperGroup = upperCellGroup.begin();
            auto currentLowerGroup = lowerCellGroup.cbegin();

            const auto endUpperGroup = upperCellGroup.end();
            const auto endLowerGroup = lowerCellGroup.cend();

            int idxUpperGroup = 0;
            int idxLowerGroup = 0;

            while(currentUpperGroup != endUpperGroup && currentLowerGroup != endLowerGroup){
                assert(spaceSystem.getParentIndex(currentLowerGroup->getStartingSpacialIndex()) <= currentUpperGroup->getEndingSpacialIndex()
                       || currentUpperGroup->getStartingSpacialIndex() <= spaceSystem.getParentIndex(currentLowerGroup->getEndingSpacialIndex()));
                auto* thisptr = this;
                unsigned char* groupCellsLowerData = inTree.getCellGroupsAtLevel(idxLevel+1)[idxLowerGroup].getDataPtr();
                size_t groupCellsLowerDataSize = inTree.getCellGroupsAtLevel(idxLevel+1)[idxLowerGroup].getDataSize();
                unsigned char* groupParticlesUpperData = inTree.getCellGroupsAtLevel(idxLevel)[idxUpperGroup].getDataPtr();
                size_t groupParticlesUpperDataSize = inTree.getCellGroupsAtLevel(idxLevel)[idxUpperGroup].getDataSize();
                starpu_insert_task(&m2m_cl,
                                   STARPU_VALUE, &thisptr, sizeof(void*),
                                   STARPU_VALUE, &idxLevel, sizeof(long int),
                                   STARPU_VALUE, &groupCellsLowerData, sizeof(void*),
                                   STARPU_VALUE, &groupCellsLowerDataSize, sizeof(size_t),
                                   STARPU_VALUE, &groupParticlesUpperData, sizeof(void*),
                                   STARPU_VALUE, &groupParticlesUpperDataSize, sizeof(size_t),
                                   STARPU_PRIORITY, priorities.getM2MPriority(idxLevel),
                                   STARPU_R, cellHandles[idxLevel+1][idxLowerGroup][0],
                                   STARPU_R, cellHandles[idxLevel+1][idxLowerGroup][1],
                                   STARPU_R, cellHandles[idxLevel][idxUpperGroup][0],
                                   starpu_data_access_mode(STARPU_RW|STARPU_COMMUTE), cellHandles[idxLevel][idxUpperGroup][1],
                                   STARPU_NAME, "M2M",
                                   0);

                if(spaceSystem.getParentIndex(currentLowerGroup->getEndingSpacialIndex()) <= currentUpperGroup->getEndingSpacialIndex()){
                    ++currentLowerGroup;
                    ++idxLowerGroup;
                    if(currentLowerGroup != endLowerGroup && currentUpperGroup->getEndingSpacialIndex() < spaceSystem.getParentIndex(currentLowerGroup->getStartingSpacialIndex())){
                        ++currentUpperGroup;
                        ++idxUpperGroup;
                    }
                }
                else{
                    ++currentUpperGroup;
                    ++idxUpperGroup;
                }
            }
        }
    }

    template <class TreeClass>
    void M2L(TreeClass& inTree, CellHandleContainer& cellHandles){
        const auto& spacialSystem = inTree.getSpacialSystem();

        for(long int idxLevel = stopUpperLevel ; idxLevel <= configuration.getTreeHeight()-1 ; ++idxLevel){
            auto& cellGroups = inTree.getCellGroupsAtLevel(idxLevel);

            auto currentCellGroup = cellGroups.begin();
            const auto endCellGroup = cellGroups.end();
            int idxGroup = 0;

            while(currentCellGroup != endCellGroup){
                auto indexesForGroup = spacialSystem.getInteractionListForBlock(*currentCellGroup, idxLevel);
                TbfAlgorithmUtils::TbfMapIndexesAndBlocksIndexes(std::move(indexesForGroup.second), cellGroups, std::distance(cellGroups.begin(),currentCellGroup),
                                               [&](auto& groupTargetIdx, const auto& groupSrcIdx, const auto& indexes){
                    auto* thisptr = this;
                    vecIndexBuffer.push_back(indexes.toStdVector());
                    VecOfIndexes* indexesForGroup_firstPtr = &vecIndexBuffer.back();
                    unsigned char* groupCellsSrcData = inTree.getCellGroupsAtLevel(idxLevel)[groupSrcIdx].getDataPtr();
                    size_t groupCellsDataSrcSize = inTree.getCellGroupsAtLevel(idxLevel)[groupSrcIdx].getDataSize();
                    unsigned char* groupCellsTgtData = inTree.getCellGroupsAtLevel(idxLevel)[groupTargetIdx].getDataPtr();
                    size_t groupCellsDataTgtSize = inTree.getCellGroupsAtLevel(idxLevel)[groupTargetIdx].getDataSize();
                    starpu_insert_task(&m2l_cl_between_groups,
                                       STARPU_VALUE, &thisptr, sizeof(void*),
                                       STARPU_VALUE, &idxLevel, sizeof(int),
                                       STARPU_VALUE, &indexesForGroup_firstPtr, sizeof(void*),
                                       STARPU_VALUE, &groupCellsSrcData, sizeof(void*),
                                       STARPU_VALUE, &groupCellsDataSrcSize, sizeof(size_t),
                                       STARPU_VALUE, &groupCellsTgtData, sizeof(void*),
                                       STARPU_VALUE, &groupCellsDataTgtSize, sizeof(size_t),
                                       STARPU_PRIORITY, priorities.getM2LPriority(idxLevel),
                                       STARPU_R, cellHandles[idxLevel][groupSrcIdx][0],
                                       STARPU_R, cellHandles[idxLevel][groupSrcIdx][1],
                                       STARPU_R, cellHandles[idxLevel][groupTargetIdx][0],
                                       starpu_data_access_mode(STARPU_RW|STARPU_COMMUTE), cellHandles[idxLevel][groupTargetIdx][2],
                                       STARPU_NAME, "M2L",
                                       0);
                });

                auto* thisptr = this;
                vecIndexBuffer.push_back(std::move(indexesForGroup.first));
                VecOfIndexes* indexesForGroup_firstPtr = &vecIndexBuffer.back();
                unsigned char* groupCellsData = inTree.getCellGroupsAtLevel(idxLevel)[idxGroup].getDataPtr();
                size_t groupCellsDataSize = inTree.getCellGroupsAtLevel(idxLevel)[idxGroup].getDataSize();
                starpu_insert_task(&m2l_cl_inside,
                                   STARPU_VALUE, &thisptr, sizeof(void*),
                                   STARPU_VALUE, &idxLevel, sizeof(int),
                                   STARPU_VALUE, &indexesForGroup_firstPtr, sizeof(void*),
                                   STARPU_VALUE, &groupCellsData, sizeof(void*),
                                   STARPU_VALUE, &groupCellsDataSize, sizeof(size_t),
                                   STARPU_PRIORITY, priorities.getM2LPriority(idxLevel),
                                   STARPU_R, cellHandles[idxLevel][idxGroup][0],
                                   STARPU_R, cellHandles[idxLevel][idxGroup][1],
                                   starpu_data_access_mode(STARPU_RW|STARPU_COMMUTE), cellHandles[idxLevel][idxGroup][2],
                                   STARPU_NAME, "M2L-IN",
                                   0);

                ++currentCellGroup;
                ++idxGroup;
            }
        }
    }

    template <class TreeClass>
    void L2L(TreeClass& inTree, CellHandleContainer& cellHandles){
        for(long int idxLevel = stopUpperLevel ; idxLevel <= configuration.getTreeHeight()-2 ; ++idxLevel){
            const auto& upperCellGroup = inTree.getCellGroupsAtLevel(idxLevel);
            auto& lowerCellGroup = inTree.getCellGroupsAtLevel(idxLevel+1);

            auto currentUpperGroup = upperCellGroup.cbegin();
            auto currentLowerGroup = lowerCellGroup.begin();

            const auto endUpperGroup = upperCellGroup.cend();
            const auto endLowerGroup = lowerCellGroup.end();

            int idxUpperGroup = 0;
            int idxLowerGroup = 0;

            while(currentUpperGroup != endUpperGroup && currentLowerGroup != endLowerGroup){
                assert(spaceSystem.getParentIndex(currentLowerGroup->getStartingSpacialIndex()) <= currentUpperGroup->getEndingSpacialIndex()
                       || currentUpperGroup->getStartingSpacialIndex() <= spaceSystem.getParentIndex(currentLowerGroup->getEndingSpacialIndex()));
                auto* thisptr = this;
                unsigned char* groupParticlesUpperData = inTree.getCellGroupsAtLevel(idxLevel)[idxUpperGroup].getDataPtr();
                size_t groupParticlesUpperDataSize = inTree.getCellGroupsAtLevel(idxLevel)[idxUpperGroup].getDataSize();
                unsigned char* groupCellsLowerData = inTree.getCellGroupsAtLevel(idxLevel+1)[idxLowerGroup].getDataPtr();
                size_t groupCellsLowerDataSize = inTree.getCellGroupsAtLevel(idxLevel+1)[idxLowerGroup].getDataSize();
                starpu_insert_task(&l2l_cl,
                                   STARPU_VALUE, &thisptr, sizeof(void*),
                                   STARPU_VALUE, &idxLevel, sizeof(long int),
                                   STARPU_VALUE, &groupParticlesUpperData, sizeof(void*),
                                   STARPU_VALUE, &groupParticlesUpperDataSize, sizeof(size_t),
                                   STARPU_VALUE, &groupCellsLowerData, sizeof(void*),
                                   STARPU_VALUE, &groupCellsLowerDataSize, sizeof(size_t),
                                   STARPU_PRIORITY, priorities.getL2LPriority(idxLevel),
                                   STARPU_R, cellHandles[idxLevel][idxUpperGroup][0],
                                   STARPU_R, cellHandles[idxLevel][idxUpperGroup][2],
                                   STARPU_R, cellHandles[idxLevel+1][idxLowerGroup][0],
                                   starpu_data_access_mode(STARPU_RW|STARPU_COMMUTE), cellHandles[idxLevel+1][idxLowerGroup][2],
                                   STARPU_NAME, "L2L",
                                   0);

                if(spaceSystem.getParentIndex(currentLowerGroup->getEndingSpacialIndex()) <= currentUpperGroup->getEndingSpacialIndex()){
                    ++currentLowerGroup;
                    ++idxLowerGroup;
                    if(currentLowerGroup != endLowerGroup && currentUpperGroup->getEndingSpacialIndex() < spaceSystem.getParentIndex(currentLowerGroup->getStartingSpacialIndex())){
                        ++currentUpperGroup;
                        ++idxUpperGroup;
                    }
                }
                else{
                    ++currentUpperGroup;
                    ++idxUpperGroup;
                }
            }
        }
    }

    template <class TreeClass>
    void L2P(TreeClass& inTree, CellHandleContainer& cellHandles, ParticleHandleContainer& particleHandles){
        if(configuration.getTreeHeight() > stopUpperLevel){
            const auto& leafGroups = inTree.getLeafGroups();
            auto& particleGroups = inTree.getParticleGroups();

            assert(std::size(leafGroups) == std::size(particleGroups));

            auto currentLeafGroup = leafGroups.cbegin();
            auto currentParticleGroup = particleGroups.begin();

            const auto endLeafGroup = leafGroups.cend();
            const auto endParticleGroup = particleGroups.end();

            int idxGroup = 0;

            while(currentLeafGroup != endLeafGroup && currentParticleGroup != endParticleGroup){
                assert((*currentParticleGroup).getStartingSpacialIndex() == (*currentLeafGroup).getStartingSpacialIndex()
                       && (*currentParticleGroup).getEndingSpacialIndex() == (*currentLeafGroup).getEndingSpacialIndex()
                       && (*currentParticleGroup).getNbLeaves() == (*currentLeafGroup).getNbCells());

                auto* thisptr = this;
                unsigned char* groupCellsData = inTree.getLeafGroups()[idxGroup].getDataPtr();
                size_t groupCellsDataSize = inTree.getLeafGroups()[idxGroup].getDataSize();
                unsigned char* groupParticlesData = inTree.getParticleGroups()[idxGroup].getDataPtr();
                size_t groupParticlesDataSize = inTree.getParticleGroups()[idxGroup].getDataSize();
                starpu_insert_task(&l2p_cl,
                                   STARPU_VALUE, &thisptr, sizeof(void*),
                                   STARPU_VALUE, &groupCellsData, sizeof(void*),
                                   STARPU_VALUE, &groupCellsDataSize, sizeof(size_t),
                                   STARPU_VALUE, &groupParticlesData, sizeof(void*),
                                   STARPU_VALUE, &groupParticlesDataSize, sizeof(size_t),
                                   STARPU_PRIORITY, priorities.getL2PPriority(),
                                   STARPU_R, cellHandles[configuration.getTreeHeight()-1][idxGroup][0],
                                   STARPU_R, cellHandles[configuration.getTreeHeight()-1][idxGroup][2],
                                   STARPU_R, particleHandles[idxGroup][0],
                                   starpu_data_access_mode(STARPU_RW|STARPU_COMMUTE), particleHandles[idxGroup][1],
                                   STARPU_NAME, "L2P",
                                   0);

                ++currentParticleGroup;
                ++currentLeafGroup;
                ++idxGroup;
            }
        }
    }

    template <class TreeClass>
    void P2P(TreeClass& inTree, ParticleHandleContainer& particleHandles){
        const auto& spacialSystem = inTree.getSpacialSystem();

        auto& particleGroups = inTree.getParticleGroups();

        auto currentParticleGroup = particleGroups.begin();
        const auto endParticleGroup = particleGroups.end();

        int idxGroup = 0;

        while(currentParticleGroup != endParticleGroup){

            auto indexesForGroup = spacialSystem.getNeighborListForBlock(*currentParticleGroup, configuration.getTreeHeight()-1, true);
            TbfAlgorithmUtils::TbfMapIndexesAndBlocksIndexes(std::move(indexesForGroup.second), particleGroups, std::distance(particleGroups.begin(), currentParticleGroup),
                                           [&](auto& groupTargetIdx, auto& groupSrcIdx, const auto& indexes){
                auto* thisptr = this;
                vecIndexBuffer.push_back(indexes.toStdVector());
                VecOfIndexes* vecIndexesPtr = &vecIndexBuffer.back();
                unsigned char* srcData = inTree.getParticleGroups()[groupSrcIdx].getDataPtr();
                size_t srcDataSize = inTree.getParticleGroups()[groupSrcIdx].getDataSize();
                unsigned char* tgtData = inTree.getParticleGroups()[groupTargetIdx].getDataPtr();
                size_t tgtDataSize = inTree.getParticleGroups()[groupTargetIdx].getDataSize();
                starpu_insert_task(&p2p_cl_twoleaves,
                                   STARPU_VALUE, &thisptr, sizeof(void*),
                                   STARPU_VALUE, &vecIndexesPtr, sizeof(void*),
                                   STARPU_VALUE, &srcData, sizeof(void*),
                                   STARPU_VALUE, &srcDataSize, sizeof(size_t),
                                   STARPU_VALUE, &tgtData, sizeof(void*),
                                   STARPU_VALUE, &tgtDataSize, sizeof(size_t),
                                   STARPU_PRIORITY, priorities.getP2PPriority(),
                                   STARPU_R, particleHandles[groupSrcIdx][0],
                                   starpu_data_access_mode(STARPU_RW|STARPU_COMMUTE), particleHandles[groupSrcIdx][1],
                                   STARPU_R, particleHandles[groupTargetIdx][0],
                                   starpu_data_access_mode(STARPU_RW|STARPU_COMMUTE), particleHandles[groupTargetIdx][1],
                                   STARPU_NAME, "P2P-INOUT",
                                   0);
            });

            auto* thisptr = this;
            vecIndexBuffer.push_back(std::move(indexesForGroup.first));
            VecOfIndexes* indexesForGroup_firstPtr = &vecIndexBuffer.back();
            unsigned char* groupData = inTree.getParticleGroups()[idxGroup].getDataPtr();
            size_t groupDataSize = inTree.getParticleGroups()[idxGroup].getDataSize();
            starpu_insert_task(&p2p_cl_oneleaf,
                               STARPU_VALUE, &thisptr, sizeof(void*),
                               STARPU_VALUE, &indexesForGroup_firstPtr, sizeof(void*),
                               STARPU_VALUE, &groupData, sizeof(void*),
                               STARPU_VALUE, &groupDataSize, sizeof(size_t),
                               STARPU_PRIORITY, priorities.getP2PPriority(),
                               STARPU_R, particleHandles[idxGroup][0],
                               starpu_data_access_mode(STARPU_RW|STARPU_COMMUTE), particleHandles[idxGroup][1],
                               STARPU_NAME, "P2P",
                               0);

            ++currentParticleGroup;
            ++idxGroup;
        }
    }

    void increaseNumberOfKernels(const int inNbThreads){
        for(long int idxThread = kernels.size() ; idxThread < inNbThreads ; ++idxThread){
            kernels.emplace_back(kernels[0]);
        }
    }

public:
    explicit TbfSmStarpuAlgorithm(const SpacialConfiguration& inConfiguration, const long int inStopUpperLevel = TbfDefaultLastLevel)
        : configuration(inConfiguration), spaceSystem(configuration), stopUpperLevel(std::max(0L, inStopUpperLevel)),
          kernelWrapper(configuration),
          priorities(configuration.getTreeHeight()){
        kernels.emplace_back(configuration);

        [[maybe_unused]] const int ret = starpu_init(NULL);
        assert(ret == 0);
        starpu_pause();
    }

    template <class SourceKernelClass,
              typename = typename std::enable_if<!std::is_same<long int, typename std::remove_const<typename std::remove_reference<SourceKernelClass>::type>::type>::value
                                                 && !std::is_same<int, typename std::remove_const<typename std::remove_reference<SourceKernelClass>::type>::type>::value, void>::type>
    TbfSmStarpuAlgorithm(const SpacialConfiguration& inConfiguration, SourceKernelClass&& inKernel, const long int inStopUpperLevel = TbfDefaultLastLevel)
        : configuration(inConfiguration), spaceSystem(configuration), stopUpperLevel(std::max(0L, inStopUpperLevel)),
          kernelWrapper(configuration),
          priorities(configuration.getTreeHeight()){
        kernels.emplace_back(std::forward<SourceKernelClass>(inKernel));

        [[maybe_unused]] const int ret = starpu_init(NULL);
        assert(ret == 0);
        starpu_pause();
    }

    ~TbfSmStarpuAlgorithm(){
        starpu_resume();
        starpu_shutdown();
    }

    template <class TreeClass>
    void execute(TreeClass& inTree, const int inOperationToProceed = TbfAlgorithmUtils::TbfOperations::TbfNearAndFarFields){
        assert(configuration == inTree.getSpacialConfiguration());

        auto allCellHandles = TbfStarPUHandleBuilder::GetCellHandles(inTree, configuration);
        auto allParticlesHandles = TbfStarPUHandleBuilder::GetParticleHandles(inTree);

        using CellContainerClass = typename TreeClass::CellGroupClass;
        using ParticleContainerClass = typename TreeClass::LeafGroupClass;

        initCodelet<CellContainerClass, ParticleContainerClass>();

        starpu_resume();

        increaseNumberOfKernels(starpu_worker_get_count_by_type(STARPU_CPU_WORKER));

        if(inOperationToProceed & TbfAlgorithmUtils::TbfP2M){
            P2M(inTree, allCellHandles, allParticlesHandles);
        }
        if(inOperationToProceed & TbfAlgorithmUtils::TbfM2M){
            M2M(inTree, allCellHandles);
        }
        if(inOperationToProceed & TbfAlgorithmUtils::TbfM2L){
            M2L(inTree, allCellHandles);
        }
        if(inOperationToProceed & TbfAlgorithmUtils::TbfL2L){
            L2L(inTree, allCellHandles);
        }
        if(inOperationToProceed & TbfAlgorithmUtils::TbfP2P){
            P2P(inTree, allParticlesHandles);
        }
        if(inOperationToProceed & TbfAlgorithmUtils::TbfL2P){
            L2P(inTree, allCellHandles, allParticlesHandles);
        }

        starpu_task_wait_for_all();

        starpu_pause();
        vecIndexBuffer.clear();
        TbfStarPUHandleBuilder::CleanCellHandles(allCellHandles);
        TbfStarPUHandleBuilder::CleanParticleHandles(allParticlesHandles);
    }

    template <class FuncType>
    auto applyToAllKernels(FuncType&& inFunc) const {
        for(const auto& kernel : kernels){
            inFunc(kernel);
        }
    }

    template <class StreamClass>
    friend  StreamClass& operator<<(StreamClass& inStream, const TbfSmStarpuAlgorithm& inAlgo) {
        inStream << "TbfSmStarpuAlgorithm @ " << &inAlgo << "\n";
        inStream << " - Configuration: " << "\n";
        inStream << inAlgo.configuration << "\n";
        inStream << " - Space system: " << "\n";
        inStream << inAlgo.spaceSystem << "\n";
        inStream << " - Total workers: " << starpu_worker_get_count() << "\n";
        inStream << "  - CPU " << starpu_cpu_worker_get_count() << "\n";
        return inStream;
    }

    static int GetNbThreads(){
        return starpu_worker_get_count_by_type(STARPU_CPU_WORKER);
    }

    static const char* GetName(){
        return "TbfSmStarpuAlgorithm";
    }
};

#endif
