// NAFEMS LE10 Benchmark solved with Sparselizard

#include "sparselizard.h"
using namespace sl;

int main(int argc, char **argv) {
  
  int bulk = 1;
  int upper = 2;
  int DCDC = 3;
  int ABAB = 4;
  int BCBC = 5;
  int midplane = 6;
  
  double young = 210e3;
  double poisson = 0.3;
  
//  mesh mymesh("petsc:le10.msh", 0);
  std::string c = (argc > 1) ? argv[1] : "1";
  mesh mymesh("../le10-"+c+".msh2", 0);
  field u("h1xyz");
  
  parameter E;
  E|bulk = young;
  parameter nu;
  nu|bulk = poisson;

  u.setorder(bulk, 2);  
  u.compy().setconstraint(DCDC); // v=0 @ DCDC
  u.compx().setconstraint(ABAB); // u=0 @ ABAB
  u.compx().setconstraint(BCBC); // u=0 @ BCBC
  u.compy().setconstraint(BCBC); // v=0 @ BCBC
  u.compz().setconstraint(midplane); // w=0 @ midplae
  
  formulation elasticity;
  elasticity += integral(upper, array1x3(0,0,-1)*tf(u));  // p=1 @ upper
  elasticity += integral(bulk, predefinedelasticity(dof(u), tf(u), E, nu));
  elasticity.generate();
  
  vec solu = solve(elasticity.A(), elasticity.b());

  // Transfer the data from the solution vector to the u field:
  u.setdata(bulk, solu);
  u.write(bulk, "le10-sparselizard-displ.vtk", 2);

  double lambda = young * poisson/((1+poisson)*(1-2*poisson));
  double mu = 0.5*young/(1+poisson);
  
  expression H(6,6,{lambda+2*mu,      lambda,      lambda,  0,  0,  0,
                    lambda,      lambda+2*mu,      lambda,  0,  0,  0,
                    lambda,           lambda, lambda+2*mu,  0,  0,  0,
                         0,                0,           0, mu,  0,  0,
                         0,                0,           0,  0,  mu, 0,
                         0,                0,           0,  0,  0, mu});

  expression stress = H*strain(u);
  comp(1, stress).write(bulk, "le10-sparselizard-sigmay.vtk", 2);   

  std::cout << comp(1, stress).interpolate(bulk, {2000, 0, 300})[0] << std::endl;
  
  return 0;
}

