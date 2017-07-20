/*
 * ModuleTesterBase.h
 *
 *  Created on: Jun 15, 2017
 *      Author: bora
 */

#ifndef INCLUDE_TEST_MODULETESTERBASE_H_
#define INCLUDE_TEST_MODULETESTERBASE_H_

class RuntimeOpts;

class ModuleTesterBase {
public:
	virtual void Execute(RuntimeOpts *opts) = 0;
};

#endif /* INCLUDE_TEST_MODULETESTERBASE_H_ */
