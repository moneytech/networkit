/*
 * NumericTools.h
 *
 *  Created on: 15.02.2013
 *      Author: cls
 */

#ifndef NUMERICTOOLS_H_
#define NUMERICTOOLS_H_

#include <limits>



namespace EnsembleClustering {

class NumericTools {
public:
	NumericTools();
	virtual ~NumericTools();

	template <typename T> bool willOverflow(const T& pX, const T& pValue, const T& pMax = std::numeric_limits<T>::max()) {
		return pMax - pValue < pX;
	}

	template <typename T> bool willUnderflow(const T& pX, const T& pValue, const T& pMin = std::numeric_limits<T>::min()) {
		return pMin + pValue > pX;
	}


	bool equal(const double x, const double y);


};

} /* namespace EnsembleClustering */
#endif /* NUMERICTOOLS_H_ */
