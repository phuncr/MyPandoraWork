/**
 *  @file   WorkshopContent/workshopcontent/Algorithms/MyShowerAlgorithm.h
 * 
 *  @brief  Header file for the myshower algorithm class.
 * 
 *  $Log: $
 */
#ifndef WORKSHOP_MYSHOWER_ALGORITHM_H
#define WORKSHOP_MYSHOWER_ALGORITHM_H 1

#include "Pandora/Algorithm.h"

namespace workshop_content
{

/**
 *  @brief  MyShowerAlgorithm class
 */
class MyShowerAlgorithm : public pandora::Algorithm
{
public:
    /**
     *  @brief  Factory class for instantiating algorithm
     */
    class Factory : public pandora::AlgorithmFactory
    {
    public:
        pandora::Algorithm *CreateAlgorithm() const;
    };

    /**
      * @brief Default constructor
      */
    MyClusterMergingAlgorithm();

private:
    pandora::StatusCode Run();
    pandora::StatusCode ReadSettings(const pandora::TiXmlHandle xmlHandle);

    // Member variables here
    std::string  m_inputClusterListName; ///< The input Cluster List Name
    unsigned int m_minClusterCaloHits;   ///< The minimum number of hits in a cluster for it to be a long cluster
    double       m_maxClusterSeperation; ///< The maximum closest distance between two clusters for them to be considered associated
};

//------------------------------------------------------------------------------------------------------------------------------------------

inline pandora::Algorithm *MyShowerAlgorithm::Factory::CreateAlgorithm() const
{
    return new MyShowerAlgorithm();
}

} // namespace workshop_content

#endif // #ifndef WORKSHOP_MYSHOWER_ALGORITHM_H
