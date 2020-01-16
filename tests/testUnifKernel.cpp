#include "spacial/tbfmortonspaceindex.hpp"
#include "spacial/tbfspacialconfiguration.hpp"
#include "utils/tbfrandom.hpp"
#include "core/tbfcellscontainer.hpp"
#include "core/tbfparticlescontainer.hpp"
#include "core/tbfparticlesorter.hpp"
#include "core/tbftree.hpp"
#include "algorithms/sequential/tbfalgorithm.hpp"
#include "algorithms/smspetabaru/tbfsmspetabarualgorithm.hpp"
#include "utils/tbftimer.hpp"

#include "kernels/unifkernel/FUnifKernel.hpp"
#include "kernels/unifkernel/FUnifKernel.hpp"

#include <iostream>


int main(){
    using RealType = double;
    const int Dim = 3;

    /////////////////////////////////////////////////////////////////////////////////////////

    const std::array<RealType, Dim> BoxWidths{{1, 1, 1}};
    const long int TreeHeight = 8;
    const std::array<RealType, Dim> inBoxCenter{{0.5, 0.5, 0.5}};

    const TbfSpacialConfiguration<RealType, Dim> configuration(TreeHeight, BoxWidths, inBoxCenter);

    /////////////////////////////////////////////////////////////////////////////////////////

    const long int NbParticles = 1000;

    TbfRandom<RealType, Dim> randomGenerator(configuration.getBoxWidths());

    std::vector<std::array<RealType, Dim+1>> particlePositions(NbParticles);

    for(long int idxPart = 0 ; idxPart < NbParticles ; ++idxPart){
        auto position = randomGenerator.getNewItem();
        particlePositions[idxPart][0] = position[0];
        particlePositions[idxPart][1] = position[1];
        particlePositions[idxPart][2] = position[2];
        particlePositions[idxPart][3] = 0.1;
    }

    /////////////////////////////////////////////////////////////////////////////////////////

    const unsigned int ORDER = 5;
    constexpr long int NbDataValuesPerParticle = Dim+1;
    constexpr long int NbRhsValuesPerParticle = 4;

    constexpr long int VectorSize = TensorTraits<ORDER>::nnodes;
    constexpr long int TransformedVectorSize = (2*ORDER-1)*(2*ORDER-1)*(2*ORDER-1);

    struct MultipoleData{
        RealType multipole_exp[NbRhsValuesPerParticle * 1 * VectorSize];
        std::complex<RealType> transformed_multipole_exp[NbRhsValuesPerParticle * 1 * TransformedVectorSize];
    };

    struct LocalData{
        RealType     local_exp[NbRhsValuesPerParticle * 1 * VectorSize];
        std::complex<RealType>     transformed_local_exp[NbRhsValuesPerParticle * 1 * TransformedVectorSize];
    };

    using MultipoleClass = MultipoleData;
    using LocalClass = LocalData;
    typedef FUnifKernel<RealType, FInterpMatrixKernelR<RealType>, ORDER> KernelClass;
    const long int inNbElementsPerBlock = 50;
    const bool inOneGroupPerParent = false;

    /////////////////////////////////////////////////////////////////////////////////////////

    TbfTimer timerBuildTree;

    TbfTree<RealType, RealType, NbDataValuesPerParticle, RealType, NbRhsValuesPerParticle, MultipoleClass, LocalClass> tree(configuration, inNbElementsPerBlock,
                                                                                TbfUtils::make_const(particlePositions), inOneGroupPerParent);

    timerBuildTree.stop();
    std::cout << "Build the tree in " << timerBuildTree.getElapsed() << std::endl;

    FInterpMatrixKernelR<RealType> interpolator;
    TbfAlgorithm<RealType, KernelClass> algorithm(configuration, KernelClass(configuration, &interpolator));
    //TbfSmSpetabaruAlgorithm<RealType, KernelClass> algorithm(configuration, KernelClass(configuration, &interpolator));

    TbfTimer timerExecute;

    algorithm.execute(tree);

    timerExecute.stop();
    std::cout << "Execute in " << timerExecute.getElapsed() << std::endl;

    /////////////////////////////////////////////////////////////////////////////////////////

    {
        std::array<RealType*, 4> particles;
        for(auto& vec : particles){
            vec = new RealType[NbParticles]();
        }
        std::array<RealType*, NbRhsValuesPerParticle> particlesRhs;
        for(auto& vec : particlesRhs){
            vec = new RealType[NbParticles]();
        }

        for(long int idxPart = 0 ; idxPart < NbParticles ; ++idxPart){
            particles[0][idxPart] = particlePositions[idxPart][0];
            particles[1][idxPart] = particlePositions[idxPart][1];
            particles[2][idxPart] = particlePositions[idxPart][2];
            particles[3][idxPart] = particlePositions[idxPart][3];
        }

        TbfTimer timerDirect;

        FP2PR::template GenericInner<RealType>( particles, particlesRhs, NbParticles);

        timerDirect.stop();

        std::cout << "Direct execute in " << timerDirect.getElapsed() << std::endl;

        //////////////////////////////////////////////////////////////////////

        std::array<RealType, 4> partcilesAccuracy;
        std::array<RealType, NbRhsValuesPerParticle> partcilesRhsAccuracy;

        tree.applyToAllLeaves([&particles,&partcilesAccuracy,&particlesRhs,&partcilesRhsAccuracy]
                              (auto&& leafHeader, const long int* particleIndexes,
                              const std::array<RealType*, 4> particleDataPtr,
                              const std::array<RealType*, NbRhsValuesPerParticle> particleRhsPtr){
            for(int idxPart = 0 ; idxPart < leafHeader.nbParticles ; ++idxPart){
                for(int idxValue = 0 ; idxValue < 4 ; ++idxValue){
                   partcilesAccuracy[idxValue] = std::max(TbfUtils::RelativeAccuracy(particleDataPtr[idxValue][idxPart],
                                                                                   particles[idxValue][particleIndexes[idxPart]]),
                                                        partcilesAccuracy[idxValue]);
                }
                for(int idxValue = 0 ; idxValue < NbRhsValuesPerParticle ; ++idxValue){
                   partcilesRhsAccuracy[idxValue] = std::max(TbfUtils::RelativeAccuracy(particleRhsPtr[idxValue][idxPart],
                                                                                   particlesRhs[idxValue][particleIndexes[idxPart]]),
                                                        partcilesRhsAccuracy[idxValue]);
                }
            }
        });

        std::cout << "Relative differences:" << std::endl;
        for(int idxValue = 0 ; idxValue < 4 ; ++idxValue){
           std::cout << "- Data " << idxValue << " = " << partcilesAccuracy[idxValue] << std::endl;
        }
        for(int idxValue = 0 ; idxValue < 4 ; ++idxValue){
           std::cout << "- Rhs " << idxValue << " = " << partcilesRhsAccuracy[idxValue] << std::endl;
        }

        //////////////////////////////////////////////////////////////////////

        for(auto& vec : particles){
            delete[] vec;
        }
        for(auto& vec : particlesRhs){
            delete[] vec;
        }
    }

    return 0;
}

