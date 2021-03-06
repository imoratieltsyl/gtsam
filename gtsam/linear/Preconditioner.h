/*
 * Preconditioner.h
 *
 *  Created on: Jun 2, 2014
 *      Author: ydjian
 */

#pragma once

#include <gtsam/base/Vector.h>
#include <boost/shared_ptr.hpp>
#include <iosfwd>
#include <map>
#include <string>

namespace gtsam {

class GaussianFactorGraph;
class KeyInfo;
class VectorValues;

/* parameters for the preconditioner */
struct GTSAM_EXPORT PreconditionerParameters {

   typedef boost::shared_ptr<PreconditionerParameters> shared_ptr;

   enum Kernel { /* Preconditioner Kernel */
     GTSAM = 0,
     CHOLMOD     /* experimental */
   } kernel_ ;

   enum Verbosity {
     SILENT = 0,
     COMPLEXITY = 1,
     ERROR = 2
   } verbosity_ ;

   PreconditionerParameters(): kernel_(GTSAM), verbosity_(SILENT) {}
   PreconditionerParameters(const PreconditionerParameters &p) : kernel_(p.kernel_), verbosity_(p.verbosity_) {}
   virtual ~PreconditionerParameters() {}

   /* general interface */
   inline Kernel kernel() const { return kernel_; }
   inline Verbosity verbosity() const { return verbosity_; }

   void print() const ;

   virtual void print(std::ostream &os) const ;

   static Kernel kernelTranslator(const std::string &s);
   static Verbosity verbosityTranslator(const std::string &s);
   static std::string kernelTranslator(Kernel k);
   static std::string verbosityTranslator(Verbosity v);

   /* for serialization */
   friend std::ostream& operator<<(std::ostream &os, const PreconditionerParameters &p);
 };

/* PCG aims to solve the problem: A x = b by reparametrizing it as
 * S^t A S y = S^t b   or   M A x = M b, where A \approx S S, or A \approx M
 * The goal of this class is to provide a general interface to all preconditioners */
class GTSAM_EXPORT Preconditioner {
public:
  typedef boost::shared_ptr<Preconditioner> shared_ptr;
  typedef std::vector<size_t> Dimensions;

  /* Generic Constructor and Destructor */
  Preconditioner() {}
  virtual ~Preconditioner() {}

  /* Computation Interfaces */

  /* implement x = S^{-1} y */
  virtual void solve(const Vector& y, Vector &x) const = 0;
//  virtual void solve(const VectorValues& y, VectorValues &x) const = 0;

  /* implement x = S^{-T} y */
  virtual void transposeSolve(const Vector& y, Vector& x) const = 0;
//  virtual void transposeSolve(const VectorValues& y, VectorValues &x) const = 0;

//  /* implement x = S^{-1} S^{-T} y */
//  virtual void fullSolve(const Vector& y, Vector &x) const = 0;
//  virtual void fullSolve(const VectorValues& y, VectorValues &x) const = 0;

  /* build/factorize the preconditioner */
  virtual void build(
    const GaussianFactorGraph &gfg,
    const KeyInfo &info,
    const std::map<Key,Vector> &lambda
    ) = 0;
};

/*******************************************************************************************/
struct GTSAM_EXPORT DummyPreconditionerParameters : public PreconditionerParameters {
  typedef PreconditionerParameters Base;
  typedef boost::shared_ptr<DummyPreconditionerParameters> shared_ptr;
  DummyPreconditionerParameters() : Base() {}
  virtual ~DummyPreconditionerParameters() {}
};

/*******************************************************************************************/
class GTSAM_EXPORT DummyPreconditioner : public Preconditioner {
public:
  typedef Preconditioner Base;
  typedef boost::shared_ptr<DummyPreconditioner> shared_ptr;

public:

  DummyPreconditioner() : Base() {}
  virtual ~DummyPreconditioner() {}

  /* Computation Interfaces for raw vector */
  virtual void solve(const Vector& y, Vector &x) const { x = y; }
//  virtual void solve(const VectorValues& y, VectorValues& x) const { x = y; }

  virtual void transposeSolve(const Vector& y, Vector& x) const { x = y; }
//  virtual void transposeSolve(const VectorValues& y, VectorValues& x) const { x = y; }

//  virtual void fullSolve(const Vector& y, Vector &x) const { x = y; }
//  virtual void fullSolve(const VectorValues& y, VectorValues& x) const { x = y; }

  virtual void build(
    const GaussianFactorGraph &gfg,
    const KeyInfo &info,
    const std::map<Key,Vector> &lambda
    )  {}
};

/*******************************************************************************************/
struct GTSAM_EXPORT BlockJacobiPreconditionerParameters : public PreconditionerParameters {
  typedef PreconditionerParameters Base;
  BlockJacobiPreconditionerParameters() : Base() {}
  virtual ~BlockJacobiPreconditionerParameters() {}
};

/*******************************************************************************************/
class GTSAM_EXPORT BlockJacobiPreconditioner : public Preconditioner {
public:
  typedef Preconditioner Base;
  BlockJacobiPreconditioner() ;
  virtual ~BlockJacobiPreconditioner() ;

  /* Computation Interfaces for raw vector */
  virtual void solve(const Vector& y, Vector &x) const;
  virtual void transposeSolve(const Vector& y, Vector& x) const ;
//  virtual void fullSolve(const Vector& y, Vector &x) const ;

  virtual void build(
    const GaussianFactorGraph &gfg,
    const KeyInfo &info,
    const std::map<Key,Vector> &lambda
    ) ;

protected:

  void clean() ;

  std::vector<size_t> dims_;
  double *buffer_;
  size_t bufferSize_;
  size_t nnz_;
};

/*********************************************************************************************/
/* factory method to create preconditioners */
boost::shared_ptr<Preconditioner> createPreconditioner(const boost::shared_ptr<PreconditionerParameters> parameters);

}


