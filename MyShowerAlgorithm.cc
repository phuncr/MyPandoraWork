/**
 *  @file   WorkshopContent/workshopcontent/Algorithms/MyShowerAlgorithm.cc
 * 
 *  @brief  Implementation of the myshower algorithm class.
 * 
 *  $Log: $
 */

#include "Pandora/AlgorithmHeaders.h"

#include "larpandoracontent/LArHelpers/LArClusterHelper.h"
#include "larpandoracontent/LArHelpers/LArMCParticleHelper.h"
#include "larpandoracontent/LArHelpers/LArPointingClusterHelper.h"
#include "larpandoracontent/LArHelpers/LArGeometryHelper.h"

#include "larpandoracontent/LArObjects/LArPointingCluster.h"
#include "larpandoracontent/LArObjects/LArTwoDSlidingFitResult.h"

#include "workshopcontent/Algorithms/MyShowerAlgorithm.h"

#include <algorithm>

using namespace pandora;
using namespace lar_content;

namespace workshop_content
{

MyShowerAlgorithm::MyShowerAlgorithm() :
    m_inputClusterListName(),
    m_minClusterCaloHits(0),
    m_maxClusterSeperation(4),
{
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode MyShowerAlgorithm::Run()
{
    // Algorithm code here
    const ClusterList *pClusterList(nullptr);
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::GetList(*this, m_inputClusterListName, pClusterList));
    std::vector<const Cluster*> clustvec1;
    ClusterVector sortedLongClusters;
    for (const Cluster *const pCluster : *pClusterList[i])
    {
        if (pCluster->GetNCaloHits() > 0) { sortedLongClusters.push_back(pCluster); }
    }
    std::sort(sortedLongClusters.begin(), sortedLongClusters.end(), LArClusterHelper::SortByNHits);

    for (const Cluster *const pParentCluster : sortedLongClusters)
    {
        for (const Cluster *const pDaughterCluster :sortedLongClusters)
        {
            if ( pParentCluster == pDaughterCluster ) { continue; }

            /* Modified Code from function LArClusterHelper::GetClosestDistance(pParentCluster, pDaughterCluster) */
            bool distanceFound(false);
            float maxDistanceSquared(0);
            CartesianVector furthestPosition1(0.f, 0.f, 0.f);
            CartesianVector furthestPosition2(0.f, 0.f, 0.f);
            const OrderedCaloHitList &orderedCaloHitList1(pParentCluster->GetOrderedCaloHitList());
            const OrderedCaloHitList &orderedCaloHitList2(pDaughterCluster->GetOrderedCaloHitList());

            // Loop over hits in cluster 1
            for (OrderedCaloHitList::const_iterator iter1 = orderedCaloHitList1.begin(), iter1End = orderedCaloHitList1.end(); iter1 != iter1End; ++iter1)
            {
                for (CaloHitList::const_iterator hitIter1 = iter1->second->begin(), hitIter1End = iter1->second->end(); hitIter1 != hitIter1End; ++hitIter1)
                {
                    const CartesianVector &positionVector1((*hitIter1)->GetPositionVector());
                    // Loop over hits in cluster 2
                    for (OrderedCaloHitList::const_iterator iter2 = orderedCaloHitList2.begin(), iter2End = orderedCaloHitList2.end(); iter2 != iter2End; ++iter2)
                    {
                        for (CaloHitList::const_iterator hitIter2 = iter2->second->begin(), hitIter2End = iter2->second->end(); hitIter2 != hitIter2End; ++hitIter2)
                        { 
                            const CartesianVector &positionVector2((*hitIter2)->GetPositionVector());
                            const float distanceSquared((positionVector1 - positionVector2).GetMagnitudeSquared());
                            if (distanceSquared > maxDistanceSquared)
                            {
                                maxDistanceSquared = distanceSquared;
                                furthestPosition1 = positionVector1;
                                furthestPosition2 = positionVector2;
                                distanceFound = true;
                            }
                        }
                    }
                }
            }
   
            if (!distanceFound) { throw StatusCodeException(STATUS_CODE_NOT_FOUND); }

            if( (closestPosition1 - closestPosition2).GetMagnitude() < m_maxClusterSeperation )
            {
              clustvec1.push_back(pParentCluster);
              clustvec1.push_back(pDaughterCluster);
              continue;
            }
        }
    }

    const std::string enlargelistname(m_inputClusterListName), deletelistname(m_inputClusterListName);
    for(int k=0; k<clustvec1.size(); k+=2)
    {
        if(clustvec1[k+1]==clustvec1[k]) continue;
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::MergeAndDeleteClusters(*this, clustvec1[k], clustvec1[k+1], enlargelistname, deletelistname));
        auto temp1 = clustvec1[k+1];
        auto temp2 = clustvec1[k];
        while(clustvec1.end() != std::find(clustvec1.begin(), clustvec1.end(), temp1))
        {    
            auto iter = std::find(clustvec1.begin(), clustvec1.end(), temp1);
            clustvec1.erase(iter);
            clustvec1.insert(iter, temp2);
        }
    }

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode MyShowerAlgorithm::ReadSettings(const TiXmlHandle xmlHandle)
{
    // Read settings from xml file here
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, XmlHelper::ReadValue(xmlHandle,
        "InputClusterListName", m_inputClusterListName));    

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "MinClusterCaloHits", m_minClusterCaloHits));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "MaxClusterSeperation", m_maxClusterSeperation));

    return STATUS_CODE_SUCCESS;
}

} // namespace workshop_content
