#ifndef NEKTAR_SOLVERS_DIFFUSION_OBLIQUE_H
#define NEKTAR_SOLVERS_DIFFUSION_OBLIQUE_H

#include "DiffBndCond.h"

namespace Nektar
{

class ObliqueBC : public DiffBndCond
{
public:
    friend class MemoryManager<ObliqueBC>;

    static DiffBndCondSharedPtr create(
        const LibUtilities::SessionReaderSharedPtr &pSession,
        const Array<OneD, MultiRegions::ExpListSharedPtr> &pFields,
        const Array<OneD, Array<OneD, NekDouble>> &pTraceNormals,
        const Array<OneD, Array<OneD, NekDouble>> &pObliqueField,
        const int pSpaceDim, const int bcRegion, const int cnt)

    {
        DiffBndCondSharedPtr p = MemoryManager<ObliqueBC>::AllocateSharedPtr(
            pSession, pFields, pTraceNormals, pObliqueField, pSpaceDim,
            bcRegion, cnt);
        return p;
    }

    static std::string className;

protected:
    void v_Apply(Array<OneD, Array<OneD, NekDouble>> &Field,
                 Array<OneD, Array<OneD, NekDouble>> &ObliqueField,
                 Array<OneD, Array<OneD, NekDouble>> &physarray,
                 const NekDouble &time) override;

private:
    ObliqueBC(const LibUtilities::SessionReaderSharedPtr &pSession,
              const Array<OneD, MultiRegions::ExpListSharedPtr> &pFields,
              const Array<OneD, Array<OneD, NekDouble>> &pTraceNormals,
              const Array<OneD, Array<OneD, NekDouble>> &pObliqueFields,
              const int pSpaceDim, const int bcRegion, const int cnt);
    ~ObliqueBC(void) override{};
};

} // namespace Nektar
#endif