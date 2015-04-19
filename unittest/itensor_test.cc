#include "test.h"
#include "itensor.h"

using namespace std;
using namespace itensor;

double static
Func(double x)
    {
    return x*x;
    }

class Functor
    {
    public:

    template<typename T>
    T
    operator()(T x) const
        {
        return x*x;
        }
    };

enum Type { NoType, Dense, Diag, DiagAllSame, Combiner };

struct GetType : RegisterFunc<GetType,Type>
    {
    template<typename T>
    Type
    operator()(const ITDense<T>& d) { return Dense; }

    template<typename T>
    Type
    operator()(const ITDiag<T>& d)
        {
        return (d.allSame() ? DiagAllSame : Diag);
        }

    Type
    operator()(const ITCombiner& c) { return Combiner; }
    };

Type
getType(const ITensor& t) 
    { 
    return applyFunc<GetType>(t.data()); 
    }

std::ostream&
operator<<(std::ostream& s, Type t)
    {
    if(t == NoType) s << "NoType";
    else if(t == Dense) s << "Dense";
    else if(t == Diag) s << "Diag";
    else if(t == DiagAllSame) s << "DiagAllSame";
    else if(t == Combiner) s << "Combiner";
    else Error("Unrecognized Type value");
    return s;
    }

TEST_CASE("ITensor")
{
Index s1("s1",2,Site);
Index s2("s2",2,Site);
Index s3("s3",2,Site);
Index s4("s4",2,Site);
Index s1P(prime(s1));
Index s2P(prime(s2));
Index s3P(prime(s3));
Index s4P(prime(s4));
Index l1("l1",2);
Index l2("l2",2);
Index l3("l3",2);
Index l4("l4",2);
Index l5("l5",2);
Index l6("l6",2);
Index l7("l7",2);
Index l8("l8",2);
Index a1("a1");
Index a2("a2");
Index a3("a3");
Index a4("a4");
Index b2("b2",2);
Index b3("b3",3);
Index b4("b4",4);
Index b5("b5",5);
Index b6("b6",6);
Index b7("b7",7);
Index b8("b8",8);

Index J("J",10),
      K("K",10),
      L("L",10);

IndexSet mixed_inds(a2,b3,l1,l2,a4,l4);
auto mixed_inds_dim = area(mixed_inds);

ITensor A,
        B,
        X,
        Z;

    {
    Matrix M(s1.m(),prime(s1).m());
    M(1,1) = 11; M(1,2) = 12;
    M(2,1) = 21; M(2,2) = 22;
    A = ITensor(s1,prime(s1),M);
    }

    {
    Matrix M(s1.m(),s2.m());
    M(1,1) = 110; M(1,2) = 120;
    M(2,1) = 210; M(2,2) = 220;
    B = ITensor(s1,s2,M);
    }

    {
    Matrix M(s1.m(),s2.m());
    M(1,1) = 0; M(1,2) = 1;
    M(2,1) = 1; M(2,2) = 0;
    X = ITensor(s1,s2,M);
    }
    
    {
    Matrix M(s1.m(),s2.m());
    M(1,1) = 1; M(1,2) =  0;
    M(2,1) = 0; M(2,2) = -1;
    Z = ITensor(s1,s2,M);
    }


SECTION("Boolean")
{
ITensor t1;

CHECK(!t1);

ITensor t2(s1);

CHECK(t2);
}

SECTION("Constructors")
{
SECTION("Rank 1")
    {
    ITensor t1(l1);
    CHECK(getType(t1) == Dense);
    CHECK_EQUAL(t1.r(),1);
    CHECK(hasindex(t1,l1));
    CHECK_CLOSE(norm(t1),0,1E-10);
    }

SECTION("Rank 2")
    {
    ITensor t2(l1,l2);
    CHECK(getType(t2) == Dense);
    CHECK_EQUAL(t2.r(),2);
    CHECK(hasindex(t2,l1));
    CHECK(hasindex(t2,l2));
    CHECK_CLOSE(norm(t2),0,1E-10);
    }

SECTION("Rank 3")
    {
    ITensor t3(l1,l2,l3);
    CHECK(getType(t3) == Dense);
    CHECK_EQUAL(t3.r(),3);
    CHECK(hasindex(t3,l1));
    CHECK(hasindex(t3,l2));
    CHECK(hasindex(t3,l3));
    CHECK_CLOSE(norm(t3),0,1E-10);
    }

SECTION("Rank 4")
    {
    ITensor t4(a1,l1);

    CHECK_EQUAL(t4.r(),2);
    CHECK(hasindex(t4,a1));
    CHECK(hasindex(t4,l1));
    CHECK_CLOSE(norm(t4),0,1E-10);
    }

SECTION("Rank 5")
    {
    ITensor t5(l1,a1,l2);

    CHECK_EQUAL(t5.r(),3);
    CHECK(hasindex(t5,a1));
    CHECK(hasindex(t5,l1));
    CHECK(hasindex(t5,l2));
    CHECK_CLOSE(norm(t5),0,1E-10);
    }

SECTION("Rank 6")
    {
    ITensor t6(l1,a1,l2,a2);

    CHECK_EQUAL(t6.r(),4);
    CHECK(hasindex(t6,l1));
    CHECK(hasindex(t6,a1));
    CHECK(hasindex(t6,l2));
    CHECK(hasindex(t6,a2));
    CHECK_CLOSE(norm(t6),0,1E-10);
    }

SECTION("Rank 7")
    {
    ITensor t7(l1,l2);
    Real a = -0.83;
    t7.fill(a);

    CHECK_EQUAL(t7.r(),2);
    CHECK(hasindex(t7,l1));
    CHECK(hasindex(t7,l2));
    CHECK_CLOSE(t7.real(l1(1),l2(1)),a,1E-5);
    CHECK_CLOSE(t7.real(l1(1),l2(2)),a,1E-5);
    CHECK_CLOSE(t7.real(l1(2),l2(1)),a,1E-5);
    CHECK_CLOSE(t7.real(l1(2),l2(2)),a,1E-5);
    t7.set(1.5,l1(2),l2(2));
    CHECK_CLOSE(t7.real(l1(2),l2(2)),1.5,1E-5);
    }


SECTION("Matrix Constructor")
    {
    Matrix M(l1.m(),b3.m()); 
    M(1,1) = 11; M(1,2) = 12; M(1,3) = 13;
    M(2,1) = 21; M(2,2) = 22; M(2,3) = 23;
    ITensor t8(l1,b3,M);

    CHECK_EQUAL(t8.r(),2);
    CHECK(hasindex(t8,l1));
    CHECK(hasindex(t8,b3));
    CHECK_CLOSE(t8.real(l1(1),b3(1)),11,1E-10);
    CHECK_CLOSE(t8.real(l1(1),b3(2)),12,1E-10);
    CHECK_CLOSE(t8.real(l1(1),b3(3)),13,1E-10);
    CHECK_CLOSE(t8.real(l1(2),b3(1)),21,1E-10);
    CHECK_CLOSE(t8.real(l1(2),b3(2)),22,1E-10);
    CHECK_CLOSE(t8.real(l1(2),b3(3)),23,1E-10);
    CHECK_CLOSE(sumels(t8),M.TreatAsVector().sumels(),1E-10);
    CHECK_CLOSE(norm(t8),Norm(M.TreatAsVector()),1E-10);

    ITensor t85(b3,l1,M.t());

    CHECK_EQUAL(t85.r(),2);
    CHECK(hasindex(t85,l1));
    CHECK(hasindex(t85,b3));
    CHECK_CLOSE(t85.real(l1(1),b3(1)),11,1E-10);
    CHECK_CLOSE(t85.real(l1(1),b3(2)),12,1E-10);
    CHECK_CLOSE(t85.real(l1(1),b3(3)),13,1E-10);
    CHECK_CLOSE(t85.real(l1(2),b3(1)),21,1E-10);
    CHECK_CLOSE(t85.real(l1(2),b3(2)),22,1E-10);
    CHECK_CLOSE(t85.real(l1(2),b3(3)),23,1E-10);
    CHECK_CLOSE(sumels(t85),M.TreatAsVector().sumels(),1E-10);
    CHECK_CLOSE(norm(t85),Norm(M.TreatAsVector()),1E-10);

    Matrix W(a1.m(),l2.m()); 
    W(1,1) = Global::random(); W(1,2) = Global::random();
    ITensor w1(a1,l2,W);

    CHECK_EQUAL(w1.r(),2);
    CHECK(hasindex(w1,a1));
    CHECK(hasindex(w1,l2));
    CHECK_CLOSE(w1.real(l2(1)),W(1,1),1E-10);
    CHECK_CLOSE(w1.real(l2(2)),W(1,2),1E-10);
    CHECK_CLOSE(sumels(w1),W.TreatAsVector().sumels(),1E-10);
    CHECK_CLOSE(norm(w1),Norm(W.TreatAsVector()),1E-10);

    ITensor w2(l2,a1,W.t());

    CHECK_EQUAL(w2.r(),2);
    CHECK(hasindex(w2,a1));
    CHECK(hasindex(w2,l2));
    CHECK_CLOSE(w2.real(l2(1)),W(1,1),1E-10);
    CHECK_CLOSE(w2.real(l2(2)),W(1,2),1E-10);
    CHECK_CLOSE(sumels(w2),W.TreatAsVector().sumels(),1E-10);
    CHECK_CLOSE(norm(w2),Norm(W.TreatAsVector()),1E-10);
    }


SECTION("Real Scalar")
    {
    Real b = -1*Global::random();
    ITensor t9(b);
    CHECK_REQUAL(sumels(t9),b);
    CHECK_REQUAL(norm(t9),fabs(b));
    }

SECTION("Dense Rank 1 from Vector")
    {
    Index linkind("linkind",10);
    Vector V(linkind.m()); 
    V.Randomize();
    ITensor t10(V,linkind);

    CHECK_EQUAL(t10.r(),1);
    CHECK(hasindex(t10,linkind));
    CHECK_CLOSE(sumels(t10),V.sumels(),1E-10);
    CHECK_CLOSE(norm(t10),Norm(V),1E-10);
    }

SECTION("Diag Rank 2 from Vector")
    {
    Index i1("i1",10),
          i2("i2",10);
    Vector V(i1.m()); 
    V.Randomize();
    ITensor T(V,i1,i2);
    CHECK(getType(T) == Diag);

    CHECK_EQUAL(T.r(),2);
    CHECK(hasindex(T,i1));
    CHECK(hasindex(T,i2));
    CHECK_CLOSE(norm(T),Norm(V),1E-10);
    CHECK_CLOSE(sumels(T),V.sumels(),1E-10);
    }
}

SECTION("IndexValConstructors")
{
SECTION("Rank 1")
    {
    ITensor t1(l1(2));
    CHECK_EQUAL(t1.r(),1);
    CHECK(hasindex(t1,l1));
    CHECK_CLOSE(t1.real(l1(1)),0,1E-10);
    CHECK_CLOSE(t1.real(l1(2)),1,1E-10);
    CHECK_CLOSE(sumels(t1),1,1E-10);
    CHECK_CLOSE(norm(t1),1,1E-10);
    }

SECTION("Rank 2")
    {
    ITensor t2(l1(2),l2(1));

    CHECK_EQUAL(t2.r(),2);
    CHECK(hasindex(t2,l1));
    CHECK(hasindex(t2,l2));
    CHECK_CLOSE(t2.real(l1(1),l2(1)),0,1E-10);
    CHECK_CLOSE(t2.real(l1(1),l2(2)),0,1E-10);
    CHECK_CLOSE(t2.real(l1(2),l2(1)),1,1E-10);
    CHECK_CLOSE(t2.real(l1(2),l2(2)),0,1E-10);
    CHECK_CLOSE(sumels(t2),1,1E-10);
    CHECK_CLOSE(norm(t2),1,1E-10);

    ITensor u2a(a1(1),l2(2));

    CHECK_EQUAL(u2a.r(),2);
    CHECK(hasindex(u2a,a1));
    CHECK(hasindex(u2a,l2));
    CHECK_CLOSE(u2a.real(l2(1)),0,1E-10);
    CHECK_CLOSE(u2a.real(l2(2)),1,1E-10);
    CHECK_CLOSE(sumels(u2a),1,1E-10);
    CHECK_CLOSE(norm(u2a),1,1E-10);

    ITensor u2b(l1(2),a2(1));

    CHECK_EQUAL(u2b.r(),2);
    CHECK(hasindex(u2b,l1));
    CHECK(hasindex(u2b,a2));
    CHECK_CLOSE(u2b.real(l1(1)),0,1E-10);
    CHECK_CLOSE(u2b.real(l1(2)),1,1E-10);
    CHECK_CLOSE(sumels(u2b),1,1E-10);
    CHECK_CLOSE(norm(u2b),1,1E-10);
    }

SECTION("Rank 3")
    {
    ITensor t3(l1(2),l3(1),l2(1));
    CHECK_EQUAL(t3.r(),3);
    CHECK(hasindex(t3,l1));
    CHECK(hasindex(t3,l2));
    CHECK(hasindex(t3,l3));
    CHECK_CLOSE(t3.real(l1(1),l3(1),l2(1)),0,1E-10);
    CHECK_CLOSE(t3.real(l1(2),l3(1),l2(1)),1,1E-10);
    CHECK_CLOSE(t3.real(l1(1),l3(2),l2(1)),0,1E-10);
    CHECK_CLOSE(t3.real(l1(2),l3(2),l2(2)),0,1E-10);
    CHECK_CLOSE(t3.real(l1(1),l3(1),l2(2)),0,1E-10);
    CHECK_CLOSE(t3.real(l1(2),l3(1),l2(2)),0,1E-10);
    CHECK_CLOSE(t3.real(l1(1),l3(2),l2(2)),0,1E-10);
    CHECK_CLOSE(t3.real(l1(2),l3(2),l2(2)),0,1E-10);
    CHECK_CLOSE(sumels(t3),1,1E-10);
    CHECK_CLOSE(norm(t3),1,1E-10);

    ITensor t4(a1(1),l3(2),l2(1));

    CHECK_EQUAL(t4.r(),3);
    CHECK(hasindex(t4,a1));
    CHECK(hasindex(t4,l2));
    CHECK(hasindex(t4,l3));
    CHECK_CLOSE(t4.real(l3(1),l2(1)),0,1E-10);
    CHECK_CLOSE(t4.real(l3(1),l2(2)),0,1E-10);
    CHECK_CLOSE(t4.real(l3(2),l2(1)),1,1E-10);
    CHECK_CLOSE(t4.real(l3(2),l2(2)),0,1E-10);
    CHECK_CLOSE(sumels(t4),1,1E-10);
    CHECK_CLOSE(norm(t4),1,1E-10);
    }

SECTION("Rank 4")
    {
    ITensor r4(l1(1),l3(1),l2(2),l4(1));

    CHECK_EQUAL(r4.r(),4);
    CHECK(hasindex(r4,l1));
    CHECK(hasindex(r4,l2));
    CHECK(hasindex(r4,l3));
    CHECK(hasindex(r4,l4));
    CHECK_CLOSE(r4.real(l1(1),l3(1),l2(2),l4(1)),1,1E-10);
    CHECK_CLOSE(sumels(r4),1,1E-10);
    CHECK_CLOSE(norm(r4),1,1E-10);
    }

SECTION("Rank 8")
    {
    ITensor t8(l1(1),l2(2),l3(1),l4(2),l5(1),l6(2),l7(1),l8(2));

    CHECK_EQUAL(t8.r(),8);
    CHECK(hasindex(t8,l1));
    CHECK(hasindex(t8,l2));
    CHECK(hasindex(t8,l3));
    CHECK(hasindex(t8,l4));
    CHECK(hasindex(t8,l5));
    CHECK(hasindex(t8,l6));
    CHECK(hasindex(t8,l7));
    CHECK(hasindex(t8,l8));

    CHECK_CLOSE(t8.real(l1(1),l2(2),l3(1),l4(2),l5(1),l6(2),l7(1),l8(2)),1,1E-10);
    CHECK_CLOSE(norm(t8),1,1E-10);
    }
}

SECTION("MultiIndexConstructors")
{
auto indices = IndexSet(a2,l3,l1,a4);

ITensor t1(indices);

CHECK_EQUAL(t1.r(),4);
CHECK(hasindex(t1,a2));
CHECK(hasindex(t1,l3));
CHECK(hasindex(t1,l1));
CHECK(hasindex(t1,a4));
CHECK_CLOSE(norm(t1),0,1E-10);

Vector V(l1.m()*l3.m());
V.Randomize();

ITensor t2(indices,V);

CHECK_EQUAL(t2.r(),4);
CHECK(hasindex(t2,a2));
CHECK(hasindex(t2,l3));
CHECK(hasindex(t2,l1));
CHECK(hasindex(t2,a4));
CHECK_CLOSE(norm(t2),Norm(V),1E-10);
CHECK_CLOSE(sumels(t2),V.sumels(),1E-10);
}

SECTION("Copy")
{
IndexSet indices(a2,l3,l1,a4);

Vector V(l1.m()*l3.m());
V.Randomize();

ITensor t1(indices,V);

CHECK_EQUAL(t1.r(),4);
CHECK(hasindex(t1,a2));
CHECK(hasindex(t1,l3));
CHECK(hasindex(t1,l1));
CHECK(hasindex(t1,a4));
CHECK_CLOSE(norm(t1),Norm(V),1E-10);
CHECK_CLOSE(sumels(t1),V.sumels(),1E-10);

//Use copy constructor
ITensor t2(t1);
t1 = ITensor(); //destroy t1

CHECK_EQUAL(t2.r(),4);
CHECK(hasindex(t2,a2));
CHECK(hasindex(t2,l3));
CHECK(hasindex(t2,l1));
CHECK(hasindex(t2,a4));
CHECK_CLOSE(norm(t2),Norm(V),1E-10);
CHECK_CLOSE(sumels(t2),V.sumels(),1E-10);

//Use operator=
ITensor t3 = t2;
t2 = ITensor(); //destroy t2

CHECK_EQUAL(t3.r(),4);
CHECK(hasindex(t3,a2));
CHECK(hasindex(t3,l3));
CHECK(hasindex(t3,l1));
CHECK(hasindex(t3,a4));
CHECK_CLOSE(norm(t3),Norm(V),1E-10);
CHECK_CLOSE(sumels(t3),V.sumels(),1E-10);
}

SECTION("ScalarMultiply")
{
A *= -1;
auto s1P = prime(s1);
CHECK_EQUAL(A.real(s1(1),s1P(1)),-11);
CHECK_EQUAL(A.real(s1(1),s1P(2)),-12);
CHECK_EQUAL(A.real(s1(2),s1P(1)),-21);
CHECK_EQUAL(A.real(s1(2),s1P(2)),-22);

Real f = Global::random();
A *= -f;
CHECK_CLOSE(A.real(s1(1),s1P(1)),11*f,1E-10);
CHECK_CLOSE(A.real(s1(1),s1P(2)),12*f,1E-10);
CHECK_CLOSE(A.real(s1(2),s1P(1)),21*f,1E-10);
CHECK_CLOSE(A.real(s1(2),s1P(2)),22*f,1E-10);

B /= f;
CHECK_CLOSE(B.real(s1(1),s2(1)),110/f,1E-10);
CHECK_CLOSE(B.real(s1(1),s2(2)),120/f,1E-10);
CHECK_CLOSE(B.real(s1(2),s2(1)),210/f,1E-10);
CHECK_CLOSE(B.real(s1(2),s2(2)),220/f,1E-10);
}

SECTION("Apply")
{
// class Functor and the function Func
// are defined in test.h

ITensor A1(A);
Functor f;
A1.apply(f);
auto s1P = prime(s1);
for(int n1 = 1; n1 <= s1.m(); ++n1)
for(int n2 = 1; n2 <= s1P.m(); ++n2)
    {
    CHECK_CLOSE( f( A.real(s1(n1),s1P(n2)) ), A1.real(s1(n1),s1P(n2)) ,1E-10);
    }

ITensor A2(A);
Real (*pFunc)(Real) = Func;
A2.apply(*pFunc);
for(int n1 = 1; n1 <= s1.m(); ++n1)
for(int n2 = 1; n2 <= s1P.m(); ++n2)
    {
    CHECK_CLOSE( Func( A.real(s1(n1),s1P(n2)) ), A2.real(s1(n1),s1P(n2)) ,1E-10);
    }
}

SECTION("SumDifference")
{
Vector V(mixed_inds_dim),
       W(mixed_inds_dim);
V.Randomize();
W.Randomize();

ITensor v(mixed_inds,V), 
        w(mixed_inds,W);

Real f1 = -Global::random(), 
     f2 = 0.1*f1;

ITensor r = f1*v + w/f2; 
for(int j1 = 1; j1 <= 2; ++j1)
for(int j2 = 1; j2 <= 2; ++j2)
for(int k3 = 1; k3 <= 3; ++k3)
for(int j4 = 1; j4 <= 2; ++j4)
    { 
    CHECK_REQUAL(r.real(l1(j1),l2(j2),b3(k3),l4(j4)),
                 f1*v.real(l1(j1),l2(j2),b3(k3),l4(j4))
               + w.real(l1(j1),l2(j2),b3(k3),l4(j4))/f2);
    }

ITensor d(v); 
d -= w;
for(int j1 = 1; j1 <= 2; ++j1)
for(int j2 = 1; j2 <= 2; ++j2)
for(int k3 = 1; k3 <= 3; ++k3)
for(int j4 = 1; j4 <= 2; ++j4)
    { 
    CHECK_REQUAL(d.real(l1(j1),l2(j2),b3(k3),l4(j4)),
                v.real(l1(j1),l2(j2),b3(k3),l4(j4))-w.real(l1(j1),l2(j2),b3(k3),l4(j4)));
    }

Vector YY(mixed_inds_dim),
       ZZ(mixed_inds_dim);
YY.Randomize();
ZZ.Randomize();

f1 = 1; f2 = 1;
ITensor yy(mixed_inds,YY), 
        zz(mixed_inds,ZZ);
r = f1*yy + f2*zz;
for(int j1 = 1; j1 <= 2; ++j1)
for(int j2 = 1; j2 <= 2; ++j2)
for(int k3 = 1; k3 <= 3; ++k3)
for(int j4 = 1; j4 <= 2; ++j4)
    { 
    CHECK_REQUAL(r.real(l1(j1),l2(j2),b3(k3),l4(j4)),
                f1*yy.real(l1(j1),l2(j2),b3(k3),l4(j4))
               +f2*zz.real(l1(j1),l2(j2),b3(k3),l4(j4)));
    }

IndexSet reordered(l2,l1,b3,a4,a2,l4);
w = ITensor(reordered,W); 
r = f1*v + w/f2; 
for(int j1 = 1; j1 <= 2; ++j1)
for(int j2 = 1; j2 <= 2; ++j2)
for(int k3 = 1; k3 <= 3; ++k3)
for(int j4 = 1; j4 <= 2; ++j4)
    { 
    CHECK_REQUAL(r.real(l1(j1),l2(j2),b3(k3),l4(j4)),
                 f1*v.real(l1(j1),l2(j2),b3(k3),l4(j4))
               + w.real(l1(j1),l2(j2),b3(k3),l4(j4))/f2);
    }

SECTION("Reordered Case 2")
    {
    auto T1 = randIT(b6,s1,b5,s2),
         T2 = randIT(s1,s2,b6,b5);
    auto R = T1+T2;
    for(int j6 = 1; j6 <= b6.m(); ++j6)
    for(int j5 = 1; j5 <= b5.m(); ++j5)
    for(int k1 = 1; k1 <= s1.m(); ++k1)
    for(int k2 = 1; k2 <= s2.m(); ++k2)
        {
        auto val = T1.real(b6(j6),s1(k1),b5(j5),s2(k2))+T2.real(b6(j6),s1(k1),b5(j5),s2(k2));
        CHECK_REQUAL(R.real(b6(j6),s1(k1),b5(j5),s2(k2)),val);
        }
    }

SECTION("Add diag")
    {
    Vector V1(std::min(l6.m(),b4.m())),
           V2(std::min(l6.m(),b4.m()));
    V1.Randomize();
    V2.Randomize();
    ITensor v1(V1,l6,b4),
            v2(V2,b4,l6);
    auto r = v1+v2;
    for(int j1 = 1; j1 <= 2; ++j1)
    for(int j2 = 1; j2 <= 4; ++j2)
        {
        //printfln("r.real(l6(%d),b4(%d)) = %.10f",j1,j2,r.real(l6(j1),b4(j2)));
        CHECK_REQUAL(r.real(l6(j1),b4(j2)),v1.real(l6(j1),b4(j2))+v2.real(l6(j1),b4(j2)));
        }
    }

}

SECTION("ContractingProduct")
{

//Check for rank 0 ITensors
SECTION("Rank 0")
    {
    Real f = Global::random();
    auto rZ = ITensor(f); 
    auto T = randIT(b2,a1,b4);

    auto res = rZ * T;

    CHECK_EQUAL(rZ.r(),0);
    CHECK_EQUAL(res.r(),3);

    for(int j2 = 1; j2 <= 2; ++j2)
    for(int j4 = 1; j4 <= 4; ++j4)
        {
        Real val = f * T.real(b2(j2),a1(1),b4(j4));
        CHECK_REQUAL(res.real(b2(j2),a1(1),b4(j4)),val);
        }
    }

auto L = randIT(b4,a1,b3,a2,b2), 
     R = randIT(b5,a1,b4,b2,b3);

SECTION("Case 1")
    {
    Real fL = Global::random(), 
         fR = Global::random();
    auto Lf = L * fL;
    auto Rf = R * fR;

    auto res1 = Lf*Rf;

    CHECK(hasindex(res1,b5));
    CHECK(hasindex(res1,a2));
    CHECK(!hasindex(res1,a1));
    CHECK(!hasindex(res1,b2));
    CHECK(!hasindex(res1,b3));
    CHECK(!hasindex(res1,b4));
    
    CHECK_EQUAL(res1.r(),2);

    for(int j5 = 1; j5 <= b5.m(); ++j5)
        {
        Real val = 0;
        for(int j2 = 1; j2 <= 2; ++j2)
        for(int j3 = 1; j3 <= 3; ++j3)
        for(int j4 = 1; j4 <= 4; ++j4)
            {
            val += L.real(b2(j2),a1(1),b3(j3),b4(j4))*fL * R.real(b5(j5),a1(1),b3(j3),b2(j2),b4(j4))*fR;
            }
        CHECK_CLOSE(res1.real(a2(1),b5(j5)),val,1E-10);
        }
    }

SECTION("Case 2")
    {
    auto res2 = R*L;

    CHECK(hasindex(res2,b5));
    CHECK(hasindex(res2,a2));
    CHECK(!hasindex(res2,a1));
    CHECK(!hasindex(res2,b2));
    CHECK(!hasindex(res2,b3));
    CHECK(!hasindex(res2,b4));

    CHECK_EQUAL(res2.r(),2);

    for(int j5 = 1; j5 <= b5.m(); ++j5)
        {
        Real val = 0;
        for(int j2 = 1; j2 <= 2; ++j2)
        for(int j3 = 1; j3 <= 3; ++j3)
        for(int j4 = 1; j4 <= 4; ++j4)
            {
            val += L.real(b2(j2),a1(1),b3(j3),b4(j4)) * R.real(b5(j5),a1(1),b3(j3),b2(j2),b4(j4));
            }
        CHECK_CLOSE(res2.real(a2(1),b5(j5)),val,1E-10);
        }
    }

ITensor Q = randIT(a1,b4,a2,b2), 
        P = randIT(a2,a3,a1);

Real fQ = Global::random(), 
     fP = Global::random();
auto Qf = Q * fQ;
auto Pf = P * fP;

SECTION("Case 3")
    {
    auto res3 = Qf*Pf;

    CHECK(hasindex(res3,b4));
    CHECK(hasindex(res3,b2));
    CHECK(hasindex(res3,a3));
    CHECK(!hasindex(res3,a1));
    CHECK(!hasindex(res3,a2));

    CHECK_EQUAL(res3.r(),3);

    for(int j2 = 1; j2 <= b2.m(); ++j2)
    for(int j4 = 1; j4 <= b4.m(); ++j4)
        {
        Real val = Q.real(a1(1),b4(j4),a2(1),b2(j2))*fQ * P.real(a2(1),a3(1),a1(1))*fP;
        CHECK_CLOSE(res3.real(b4(j4),b2(j2)),val,1E-10);
        }
    }

SECTION("Case 4")
    {
    auto res4 = Pf*Qf;

    CHECK(hasindex(res4,b4));
    CHECK(hasindex(res4,b2));
    CHECK(hasindex(res4,a3));
    CHECK(!hasindex(res4,a1));
    CHECK(!hasindex(res4,a2));

    CHECK_EQUAL(res4.r(),3);

    for(int j2 = 1; j2 <= 2; ++j2)
    for(int j4 = 1; j4 <= 4; ++j4)
        {
        Real val = Q.real(a1(1),b4(j4),a2(1),b2(j2))*fQ * P.real(a2(1),a3(1),a1(1))*fP;
        CHECK_CLOSE(res4.real(b4(j4),b2(j2)),val,1E-10);
        }
    }


SECTION("Case 5")
    {
    auto psi = randIT(a1,a2,a3), 
         mpoh = randIT(l2,a1,prime(a1),a2,prime(a2));

    auto Hpsi = mpoh * psi;

    CHECK_EQUAL(Hpsi.r(),4);
    CHECK(hasindex(Hpsi,l2));
    CHECK(hasindex(Hpsi,prime(a1)));
    CHECK(hasindex(Hpsi,prime(a2)));
    CHECK(hasindex(Hpsi,a3));
    CHECK(!hasindex(Hpsi,a1));
    CHECK(!hasindex(Hpsi,a2));
    }

SECTION("Case 6")
    {
    auto T1 = randIT(b3,b5,l6,a1,s3),
         T2 = randIT(l6,s4,b3,a1);
    auto R = T1*T2;
    for(int j5 = 1; j5 <= 5; ++j5)
    for(int i3 = 1; i3 <= 2; ++i3)
    for(int i4 = 1; i4 <= 2; ++i4)
        {
        Real val = 0;
        for(int j3 = 1; j3 <= 3; ++j3)
        for(int k6 = 1; k6 <= 2; ++k6)
            {
            val += T1.real(b3(j3),b5(j5),l6(k6),s3(i3)) * T2.real(l6(k6),s4(i4),b3(j3));
            }
        CHECK_CLOSE(R.real(b5(j5),s3(i3),s4(i4)),val,1E-10);
        }
    }

SECTION("Scalar Result")
    {
    auto T1 = randIT(a1,b3,b4),
         T2 = randIT(b4,a1,b3);
    auto f = -0.2342;
    T1 *= f;
    auto R = T1*T2;

    Real val = 0;
    for(long j3 = 1; j3 <= b3.m(); ++j3)
    for(long j4 = 1; j4 <= b4.m(); ++j4)
        {
        val += T1.real(a1(1),b3(j3),b4(j4))*T2.real(a1(1),b3(j3),b4(j4));
        }
    CHECK_REQUAL(val,R.real());
    }
}

//SECTION("TieIndices")
//    {
//
//    Index t("tied",2);
//
//    ITensor dX(X);
//    dX.tieIndices(s1,s2,t);
//
//    CHECK_CLOSE(norm(dX),0,1E-5);
//    CHECK_EQUAL(dX.r(),1);
//    CHECK(hasindex(dX,t));
//
//    ITensor dZ(Z);
//    dZ.tieIndices(s1,s2,t);
//    CHECK_CLOSE(dZ(t(1)),+1,1E-5);
//    CHECK_CLOSE(dZ(t(2)),-1,1E-5);
//
//    {
//    ITensor T(l1,l2,a1,s2,s1);
//    T.randomize();
//
//    ITensor TT(T);
//    TT.tieIndices(l2,l1,s1,l2);
//
//    CHECK_EQUAL(TT.r(),3);
//
//    for(int j = 1; j <= 2; ++j)
//    for(int k = 1; k <= 2; ++k)
//        {
//        CHECK_CLOSE(T(l1(j),l2(j),a1(1),s2(k),s1(j)),TT(l2(j),s2(k),a1(1)),1E-5);
//        }
//    }
//
//    //Try tying m==1 inds
//    {
//    ITensor T(l1,a2,a1,s2,a3);
//    T.randomize();
//
//    ITensor TT(T);
//    TT.tieIndices(a1,a3,a2,a1);
//
//    CHECK_EQUAL(TT.r(),3);
//
//    for(int j = 1; j <= 2; ++j)
//    for(int k = 1; k <= 2; ++k)
//        {
//        CHECK_CLOSE(T(l1(j),s2(k)),TT(l1(j),s2(k)),1E-5);
//        }
//    }
//
//
//
//    } //TieIndices
//
//SECTION("ComplexTieIndices")
//    {
//    ITensor Tr(l1,l2,a1,s2,s1),
//            Ti(l1,l2,a1,s2,s1);
//    Tr.randomize();
//    Ti.randomize();
//
//    ITensor T = Tr + Complex_i*Ti;
//
//    ITensor TT(T);
//    TT.tieIndices(l2,l1,s1,l2);
//
//    CHECK_EQUAL(TT.r(),3);
//
//    ITensor TTr(realPart(TT)),
//            TTi(imagPart(TT));
//
//    for(int j = 1; j <= 2; ++j)
//    for(int k = 1; k <= 2; ++k)
//        {
//        CHECK_CLOSE(Tr(l1(j),l2(j),a1(1),s2(k),s1(j)),TTr(l2(j),s2(k),a1(1)),1E-5);
//        CHECK_CLOSE(Ti(l1(j),l2(j),a1(1),s2(k),s1(j)),TTi(l2(j),s2(k),a1(1)),1E-5);
//        }
//    }
//
//SECTION("Trace")
//    {
//
//    ITensor A(b2,a1,b3,b5,prime(b3));
//    A.randomize();
//    Real f = -Global::random();
//    A *= f;
//
//    ITensor At = trace(A,b3,prime(b3));
//
//    for(int j2 = 1; j2 <= b2.m(); ++j2)
//    for(int j5 = 1; j5 <= b5.m(); ++j5)
//        {
//        Real val = 0;
//        for(int j3 = 1; j3 <= b3.m(); ++j3)
//            {
//            val += A(b2(j2),a1(1),b3(j3),b5(j5),prime(b3)(j3));
//            }
//        CHECK_CLOSE(val,At(b2(j2),a1(1),b5(j5)),1E-10);
//        }
//
//    ITensor MM(b5,prime(b5));
//    MM.randomize();
//    MM *= -2.34;
//
//    Real tr = trace(MM);
//
//    Real check_tr = 0;
//    for(int j5 = 1; j5 <= b5.m(); ++j5)
//        {
//        check_tr += MM(b5(j5),prime(b5)(j5));
//        }
//    CHECK_CLOSE(tr,check_tr,1E-10);
//
//    }
//
//SECTION("fromMatrix11")
//    {
//    Matrix M22(s1.m(),s2.m());
//
//    M22(1,1) = -0.3; M22(1,2) = 110;
//    M22(2,1) = -1.7; M22(1,2) = 5;
//
//    ITensor T(s1,s2);
//    //T should be overwritten so check
//    //that scalar mult has no effect
//    T *= -5; 
//
//    T.fromMatrix11(s1,s2,M22);
//
//    CHECK_CLOSE(T(s1(1),s2(1)),M22(1,1),1E-10);
//    CHECK_CLOSE(T(s1(1),s2(2)),M22(1,2),1E-10);
//    CHECK_CLOSE(T(s1(2),s2(1)),M22(2,1),1E-10);
//    CHECK_CLOSE(T(s1(2),s2(2)),M22(2,2),1E-10);
//
//    ITensor U(T);
//
//    U.fromMatrix11(s2,s1,M22);
//
//    CHECK_CLOSE(T(s1(1),s2(1)),M22(1,1),1E-10);
//    CHECK_CLOSE(T(s1(1),s2(2)),M22(1,2),1E-10);
//    CHECK_CLOSE(T(s1(2),s2(1)),M22(2,1),1E-10);
//    CHECK_CLOSE(T(s1(2),s2(2)),M22(2,2),1E-10);
//
//    CHECK_CLOSE(U(s2(1),s1(1)),M22(1,1),1E-10);
//    CHECK_CLOSE(U(s2(1),s1(2)),M22(1,2),1E-10);
//    CHECK_CLOSE(U(s2(2),s1(1)),M22(2,1),1E-10);
//    CHECK_CLOSE(U(s2(2),s1(2)),M22(2,2),1E-10);
//
//    Matrix M12(a1.m(),s2.m());
//    M12(1,1) = 37; M12(1,2) = -2;
//
//    ITensor P(a1,s2);
//    P *= -4;
//
//    P.fromMatrix11(a1,s2,M12);
//
//    CHECK_CLOSE(P(a1(1),s2(1)),M12(1,1),1E-10);
//    CHECK_CLOSE(P(a1(1),s2(2)),M12(1,2),1E-10);
//
//    P.fromMatrix11(s2,a1,M12.t());
//
//    CHECK_CLOSE(P(s2(1),a1(1)),M12(1,1),1E-10);
//    CHECK_CLOSE(P(s2(2),a1(1)),M12(1,2),1E-10);
//    }
//
//SECTION("ToFromMatrix11")
//    {
//    Matrix M(s1.m(),s2.m());    
//
//    Real f = -Global::random();
//
//    A *= f;
//
//    A.toMatrix11(s2,s1,M);
//
//    CHECK_CLOSE(M(1,1),11*f,1E-10);
//    CHECK_CLOSE(M(2,1),12*f,1E-10);
//    CHECK_CLOSE(M(1,2),21*f,1E-10);
//    CHECK_CLOSE(M(2,2),22*f,1E-10);
//
//    A.toMatrix11(s1,s2,M);
//
//    CHECK_CLOSE(M(1,1),11*f,1E-10);
//    CHECK_CLOSE(M(1,2),12*f,1E-10);
//    CHECK_CLOSE(M(2,1),21*f,1E-10);
//    CHECK_CLOSE(M(2,2),22*f,1E-10);
//
//    A.toMatrix11NoScale(s2,s1,M);
//
//    CHECK_CLOSE(M(1,1),11,1E-10);
//    CHECK_CLOSE(M(2,1),12,1E-10);
//    CHECK_CLOSE(M(1,2),21,1E-10);
//    CHECK_CLOSE(M(2,2),22,1E-10);
//
//    A *= -40;
//    A.fromMatrix11(s2,s1,M);
//
//    CHECK_CLOSE(A(s1(1),s2(1)),11,1E-10);
//    CHECK_CLOSE(A(s1(1),s2(2)),12,1E-10);
//    CHECK_CLOSE(A(s1(2),s2(1)),21,1E-10);
//    CHECK_CLOSE(A(s1(2),s2(2)),22,1E-10);
//
//    A.fromMatrix11(s1,s2,M);
//
//    CHECK_CLOSE(A(s1(1),s2(1)),11,1E-10);
//    CHECK_CLOSE(A(s1(1),s2(2)),21,1E-10);
//    CHECK_CLOSE(A(s1(2),s2(1)),12,1E-10);
//    CHECK_CLOSE(A(s1(2),s2(2)),22,1E-10);
//
//
//    Vector V(4);
//    V(1) = 3.14; V(2) = 2.718; V(3) = -1; V(4) = 0;
//    Index link("link",4);
//
//    ITensor T(link,a1);
//    T(link(1),a1(1)) = V(1);
//    T(link(2),a1(1)) = V(2);
//    T(link(3),a1(1)) = V(3);
//    T(link(4),a1(1)) = V(4);
//
//    Matrix M41(4,1), M14(1,4);
//    
//    T.toMatrix11(link,a1,M41);
//
//    CHECK_CLOSE(M41(1,1),V(1),1E-10);
//    CHECK_CLOSE(M41(2,1),V(2),1E-10);
//    CHECK_CLOSE(M41(3,1),V(3),1E-10);
//    CHECK_CLOSE(M41(4,1),V(4),1E-10);
//     
//    T.toMatrix11(a1,link,M14);
//
//    CHECK_CLOSE(M14(1,1),V(1),1E-10);
//    CHECK_CLOSE(M14(1,2),V(2),1E-10);
//    CHECK_CLOSE(M14(1,3),V(3),1E-10);
//    CHECK_CLOSE(M14(1,4),V(4),1E-10);
//
//    }
//
//SECTION("ToFromMatrix22")
//    {
//    Index i1("i1",3),
//          i2("i2",4),
//          i3("i3",2),
//          i4("i4",4);
//
//    ITensor T(i1,i2,i3,i4);
//    T.randomize();
//    T *= -1.23235;
//
//    Matrix M;
//    T.toMatrix22(i2,i1,i4,i3,M);
//    ITensor V;
//    V.fromMatrix22(i2,i1,i4,i3,M);
//
//    CHECK(norm(T-V) < 1E-12);
//    }
//
//SECTION("CommaAssignment")
//    {
//    ITensor VV(s1);
//    VV.randomize();
//    VV *= -1;
//    commaInit(VV,s1) << 1, 2;
//    CHECK_EQUAL(VV(s1(1)),1);
//    CHECK_EQUAL(VV(s1(2)),2);
//
//    ITensor ZZ(s1,s2);
//    commaInit(ZZ,s1,s2) << 1, 0, 
//                           0, -1;
//    CHECK_EQUAL(ZZ(s1(1),s2(1)),1);
//    CHECK_EQUAL(ZZ(s1(2),s2(1)),0);
//    CHECK_EQUAL(ZZ(s1(1),s2(2)),0);
//    CHECK_EQUAL(ZZ(s1(2),s2(2)),-1);
//
//    ITensor XX(s1,s2);
//    XX(s1(2),s2(1)) = 5;
//    XX *= 3;
//    commaInit(XX,s1,s2) << 0, 1, 
//                           1, 0;
//    CHECK_EQUAL(XX(s1(1),s2(1)),0);
//    CHECK_EQUAL(XX(s1(2),s2(1)),1);
//    CHECK_EQUAL(XX(s1(1),s2(2)),1);
//    CHECK_EQUAL(XX(s1(2),s2(2)),0);
//
//    ITensor AA(s1,s2);
//    AA.randomize();
//    AA *= -Global::random();
//    commaInit(AA,s1,s2) << 11, 12, 
//                           21, 22;
//    CHECK_EQUAL(AA(s1(1),s2(1)),11);
//    CHECK_EQUAL(AA(s1(1),s2(2)),12);
//    CHECK_EQUAL(AA(s1(2),s2(1)),21);
//    CHECK_EQUAL(AA(s1(2),s2(2)),22);
//
//    ITensor T(s1,s2,s3);
//    T.randomize();
//    T *= -Global::random();
//    commaInit(T,s1,s2,s3) << 111, 112, 
//                             121, 122,
//                             211, 212,
//                             221, 222;
//    CHECK_EQUAL(T(s1(1),s2(1),s3(1)),111);
//    CHECK_EQUAL(T(s1(1),s2(1),s3(2)),112);
//    CHECK_EQUAL(T(s1(1),s2(2),s3(1)),121);
//    CHECK_EQUAL(T(s1(1),s2(2),s3(2)),122);
//    CHECK_EQUAL(T(s1(2),s2(1),s3(1)),211);
//    CHECK_EQUAL(T(s1(2),s2(1),s3(2)),212);
//    CHECK_EQUAL(T(s1(2),s2(2),s3(1)),221);
//    CHECK_EQUAL(T(s1(2),s2(2),s3(2)),222);
//    }
//
//SECTION("RealImagPart")
//    {
//    const Real f1 = 2.124,
//               f2 = 1.113;
//    ITensor ZiX = f1*Complex_1*Z + f2*Complex_i*X;
//    ITensor R(realPart(ZiX)),
//            I(imagPart(ZiX));
//    R -= f1*Z;
//    I -= f2*X;
//    CHECK_CLOSE(norm(R),0,1E-5);
//    CHECK_CLOSE(norm(I),0,1E-5);
//
//    //Test hc:
//    
//    ZiX.dag();
//    R = realPart(ZiX);
//    I = imagPart(ZiX);
//    R -= f1*Z;
//    I += f2*X;
//    CHECK_CLOSE(norm(R),0,1E-5);
//    CHECK_CLOSE(norm(I),0,1E-5);
//    }

SECTION("SwapPrimeTest")
{
CHECK_EQUAL(A.real(s1(1),prime(s1)(1)),11);
CHECK_EQUAL(A.real(s1(2),prime(s1)(1)),21);
CHECK_EQUAL(A.real(s1(1),prime(s1)(2)),12);
CHECK_EQUAL(A.real(s1(2),prime(s1)(2)),22);

A = swapPrime(A,0,1);

CHECK_EQUAL(A.real(prime(s1)(1),s1(1)),11);
CHECK_EQUAL(A.real(prime(s1)(2),s1(1)),21);
CHECK_EQUAL(A.real(prime(s1)(1),s1(2)),12);
CHECK_EQUAL(A.real(prime(s1)(2),s1(2)),22);
}

SECTION("NoprimeTest")
{
ITensor T(s1,prime(s1));

//Check that T.noprime()
//throws an exception since it would
//lead to duplicate indices
CHECK_THROWS_AS(T.noprime(),ITError);
}

//SECTION("NormTest")
//    {
//    A = randIT(s1,prime(s1));
//    CHECK_CLOSE(norm(A),sqrt((A*A).real()),1E-5);
//
//    ITensor C = Complex_1*A+Complex_i*B;
//
//    CHECK_CLOSE(norm(C),sqrt(realPart(dag(C)*C).toReal()),1E-5);
//    }

//SECTION("CR_ComplexAddition")
//    {
//    const Real f1 = 1.234,
//               f2 = 2.456;
//    ITensor iZX = f1*Complex_i*Z + f2*Complex_1*X;
//    ITensor R(realPart(iZX)),
//            I(imagPart(iZX));
//    R -= f2*X;
//    I -= f1*Z;
//    CHECK_CLOSE(norm(R),0,1E-5);
//    CHECK_CLOSE(norm(I),0,1E-5);
//    }
//
//SECTION("CC_ComplexAddition")
//    {
//    const Real f1 = 1.234,
//               f2 = 2.456;
//    ITensor iZiX = f1*Complex_i*Z + f2*Complex_i*X;
//    ITensor R(realPart(iZiX)),
//            I(imagPart(iZiX));
//    I -= f1*Z+f2*X;
//    CHECK_CLOSE(norm(R),0,1E-5);
//    CHECK_CLOSE(norm(I),0,1E-5);
//    }
//
//SECTION("ComplexScalar")
//    {
//    ITensor A(b4,s1),
//            B(b4,s1);
//    A.randomize();
//    B.randomize();
//    
//    const Real f1 = 2.1324,
//               f2 = -5.2235;
//
//    ITensor T1 = Complex(f1,0)*A;
//
//    CHECK(norm(realPart(T1)-(f1*A)) < 1E-12);
//    CHECK(norm(imagPart(T1)) < 1E-12);
//
//    ITensor T2 = Complex(0,f2)*A;
//
//    CHECK(norm(realPart(T2)) < 1E-12);
//    CHECK(norm(imagPart(T2)-f2*A) < 1E-12);
//
//    ITensor T3 = Complex(f1,f2)*A;
//    CHECK(norm(realPart(T3)-f1*A)) < 1E-12);
//    CHECK(norm(imagPart(T3)-f2*A)) < 1E-12);
//
//    ITensor T4 = Complex(f2,f1)*A;
//    CHECK(norm(realPart(T4)-f2*A)) < 1E-12);
//    CHECK(norm(imagPart(T4)-f1*A)) < 1E-12);
//
//    ITensor T5 = A+Complex_i*B;
//    T5 *= Complex(f1,f2);
//    CHECK(norm(realPart(T5)-(f1*A-f2*B))) < 1E-12);
//    CHECK(norm(imagPart(T5)-(f2*A+f1*B))) < 1E-12);
//
//    ITensor T6 = A+Complex_i*B;
//    T6 *= Complex(f2,f1);
//    CHECK(norm(realPart(T6)-(f2*A-f1*B))) < 1E-12);
//    CHECK(norm(imagPart(T6)-(f1*A+f2*B))) < 1E-12);
//    }

SECTION("CommonIndex")
{
ITensor T1(s1,s2,l1,l2),
        T2(s1,l3),
        T3(s3,l4);

CHECK(hasindex(T1,s1));
CHECK(hasindex(T2,s1));

Index c = commonIndex(T1,T3);
CHECK(!c);

c = commonIndex(T2,T3);
CHECK(!c);

CHECK(commonIndex(T1,T2) == s1);
CHECK(commonIndex(T1,T2,Site) == s1);
}

SECTION("Diag ITensor Contraction")
{
SECTION("Diag All Same")
    {
    auto op = ITensor(1.,s1,a1); //all diag elements same
    CHECK(getType(op) == DiagAllSame);

    auto r1 = randIT(s1,prime(s1,2));
    auto res1 = op*r1;
    CHECK(hasindex(res1,a1));
    CHECK(hasindex(res1,prime(s1,2)));
    for(int j1 = 1; j1 <= s1.m(); ++j1)
        {
        CHECK_REQUAL(res1.real(prime(s1,2)(j1),a1(1)), r1.real(prime(s1,2)(j1),s1(1)));
        }
    }

SECTION("Diag")
    {
    Vector v(2);
    v(1) = 1.23234;
    v(2) = -0.9237;
    auto op = ITensor(v,s1,b2);
    CHECK(getType(op) == Diag);

    auto r2 = randIT(s1,s2);
    auto res2 = op*r2;
    CHECK(hasindex(res2,s2));
    CHECK(hasindex(res2,b2));
    auto diagm = std::min(s1.m(),b2.m());
    for(int j2 = 1; j2 <= s2.m(); ++j2)
    for(int d = 1; d <= diagm; ++d)
        {
        CHECK_REQUAL(res2.real(s2(j2),b2(d)), v(d) * r2.real(s2(j2),s1(d)));
        }
    }

SECTION("Trace")
    {
    auto T = randIT(s1,s2,s3);
    auto d = ITensor(1,s1,s2);
    auto R = d*T;
    for(int i3 = 1; i3 <= s3.m(); ++i3)
        {
        Real val = 0;
        for(int i12 = 1; i12 <= s1.m(); ++i12)
            {
            val += T.real(s1(i12),s2(i12),s3(i3));
            }
        CHECK_REQUAL(val,R.real(s3(i3)));
        }
    }

SECTION("Tie Indices with Diag Tensor")
    {
    auto T = randIT(s1,s2,s3,s4);

    auto tied1 = Index("tied1",s1.m());
    auto tt1 = ITensor(1,s1,s2,s3,tied1);
    auto R1 = T*tt1;
    for(int t = 1; t <= tied1.m(); ++t)
    for(int j4 = 1; j4 <= s4.m(); ++j4)
        {
        CHECK_REQUAL(T.real(s1(t),s2(t),s3(t),s4(j4)), R1.real(tied1(t),s4(j4)));
        }

    auto tied2 = Index("tied2",s1.m());
    auto tt2 = ITensor(1,s1,s3,tied2);
    auto R2 = T*tt2;
    for(int t = 1; t <= tied1.m(); ++t)
    for(int j2 = 1; j2 <= s2.m(); ++j2)
    for(int j4 = 1; j4 <= s4.m(); ++j4)
        {
        CHECK_REQUAL(T.real(s1(t),s2(j2),s3(t),s4(j4)), R2.real(tied2(t),s2(j2),s4(j4)));
        }
    }

SECTION("Contract All Dense Inds; Diag Scalar result")
    {
    auto T = randIT(J,K);

    auto d1 = ITensor(1,J,K);
    auto R = d1*T;
    CHECK(getType(R) == DiagAllSame);
    Real val = 0;
    auto minjk = std::min(J.m(),K.m());
    for(long j = 1; j <= minjk; ++j)
        val += T.real(J(j),K(j));
    CHECK_REQUAL(R.real(),val);

    Vector v(minjk);
    for(int i = 1; i <= minjk; ++i) v(i) = Global::random();
    auto d2 = ITensor(v,J,K);
    R = d2*T;
    CHECK(getType(R) == DiagAllSame);
    val = 0;
    for(long j = 1; j <= minjk; ++j)
        val += v(j)*T.real(J(j),K(j));
    CHECK_REQUAL(R.real(),val);
    }

SECTION("Contract All Dense Inds; Diag result")
    {
    auto T = randIT(J,K);
    
    auto d = ITensor(1,J,K,L);
    auto R = d*T;
    CHECK(getType(R) == Diag);
    CHECK(hasindex(R,L));
    auto minjkl = std::min(std::min(J.m(),K.m()),L.m());
    for(long j = 1; j <= minjkl; ++j)
        CHECK_REQUAL(R.real(L(j)), T.real(J(j),K(j)));
    }
}

//SECTION("Complex Diag ITensor")
//    {
//    Vector v(3);
//    v(1) = -0.8;
//    v(2) = 1.7;
//    v(3) = 4.9;
//
//    Vector vb(2);
//    vb(1) = 1;
//    vb(2) = -1;
//
//    const Real f1 = Global::random(),
//               f2 = Global::random();
//
//    ITensor op1(s1,prime(s1),f1),
//            op2(s1,prime(s1),f2),
//            opa(s1,a1,3.1),
//            psi(s1,l1,-1),
//            opb(s1,b2,vb);
//
//    auto r1 = randIT(s1,prime(s1,2)),
//         r2 = randIT(s1,prime(s1,2));
//
//    auto op3 = op1 + Complex_i*op2;
//
//    auto res1 = op1*r1;
//    res1.mapprime(1,0);
//    ITensor diff1 = res1-f1*r1;
//    CHECK(norm(diff1) < 1E-10);
//
//    auto res2 = r1*op1;
//    res2.mapprime(1,0);
//    ITensor diff2 = res2-f1*r1;
//    CHECK(norm(diff2) < 1E-10);
//
//    auto rc = r1+Complex_i*r2;
//
//    ITensor res3 = rc*op1;
//    res3.mapprime(1,0);
//    CHECK(isComplex(res3));
//    ITensor diff3 = res3-f1*rc;
//    CHECK(norm(diff3) < 1E-10);
//
//    ITensor res4 = op1*rc;
//    res4.mapprime(1,0);
//    CHECK(isComplex(res4));
//    ITensor diff4 = res4-f1*rc;
//    CHECK(norm(diff4) < 1E-10);
//
//    ITensor res5 = rc*op3;
//    CHECK(isComplex(res5));
//    ITensor rres5(realPart(res5)),
//            ires5(imagPart(res5));
//    ITensor rdiff5 = rres5-(r1*op1-r2*op2),
//            idiff5 = ires5-(r1*op2+r2*op1);
//    CHECK(norm(rdiff5) < 1E-10);
//    CHECK(norm(idiff5) < 1E-10);
//    }

//SECTION("DiagMethod")
//    {
//    ITensor t1(b3,b4);
//    t1.randomize();
//    t1 *= -8.232244;
//    Vector d1 = t1.diag();
//    for(int i = 1; i <= minM(t1.indices()); ++i)
//        {
//        CHECK(fabs(d1(i)-t1(b3(i),b4(i))) < 1E-12);
//        }
//
//    Vector v(4);
//    v(1) = -2.2442;
//    v(2) = 1.34834;
//    v(3) = 0.0;
//    v(4) = 8.38457;
//    ITensor t2(prime(b4),b4,v);
//    CHECK(t2.type() == ITensor::Diag);
//    CHECK(Norm(v-t2.diag()) < 1E-12);
//    }

SECTION("Kronecker Delta Tensor")
    {
    auto d = delta(s1,s2);
    CHECK(getType(d) == Combiner);

    auto T1 = randIT(s1,s3);

    auto R1a = d*T1;
    CHECK(R1a.r() == 2);
    CHECK(hasindex(R1a,s2));

    auto R1b = T1*d;
    CHECK(R1b.r() == 2);
    CHECK(hasindex(R1b,s2));

    for(int i3 = 1; i3 <= s3.m(); ++i3)
    for(int i12 = 1; i12 <= s1.m(); ++i12)
        {
        CHECK_REQUAL(T1.real(s1(i12),s3(i3)), R1a.real(s2(i12),s3(i3)));
        CHECK_REQUAL(T1.real(s1(i12),s3(i3)), R1b.real(s2(i12),s3(i3)));
        }

    auto T2 = randIT(s2,s3);

    auto R2a = d*T2;
    CHECK(R2a.r() == 2);
    CHECK(hasindex(R2a,s1));

    auto R2b = T2*d;
    CHECK(R2b.r() == 2);
    CHECK(hasindex(R2b,s1));

    for(int i3 = 1; i3 <= s3.m(); ++i3)
    for(int i12 = 1; i12 <= s1.m(); ++i12)
        {
        CHECK_REQUAL(T2.real(s2(i12),s3(i3)), R2a.real(s1(i12),s3(i3)));
        CHECK_REQUAL(T2.real(s2(i12),s3(i3)), R2b.real(s1(i12),s3(i3)));
        }

    auto T3 = randIT(b8,s1,b6,a1);
    auto R3a = d*T3;
    auto R3b = T3*d;
    CHECK(hasindex(R3a,s2));
    CHECK(hasindex(R3b,s2));

    auto T4 = randIT(b8,s2,b6,a1);
    auto R4a = d*T4;
    auto R4b = T4*d;
    CHECK(hasindex(R4a,s1));
    CHECK(hasindex(R4b,s1));
    }


SECTION("Combiner")
    {
    SECTION("Two Index")
        {
        auto C = combiner(s1,s2);
        CHECK(getType(C) == Combiner);

        auto T1 = randIT(s1,s2,s3);
        auto R1 = C*T1;
        auto ci = commonIndex(C,R1);
        CHECK(ci);
        CHECK(ci.m() == s1.m()*s2.m());

        for(int i1 = 1; i1 <= s1.m(); ++i1)
        for(int i2 = 1; i2 <= s2.m(); ++i2)
        for(int i3 = 1; i3 <= s3.m(); ++i3)
            {
            auto j = i1+(i2-1)*s2.m();
            CHECK_REQUAL(T1.real(s1(i1),s2(i2),s3(i3)), R1.real(ci(j),s3(i3)));
            }

        auto T2 = randIT(s1,s3,s2);
        auto R2 = C*T2;
        CHECK(R2.r() == 2);
        ci = commonIndex(C,R2);
        CHECK(ci);
        CHECK(ci.m() == s1.m()*s2.m());
        for(int i1 = 1; i1 <= s1.m(); ++i1)
        for(int i2 = 1; i2 <= s2.m(); ++i2)
        for(int i3 = 1; i3 <= s3.m(); ++i3)
            {
            auto j = i1+(i2-1)*s2.m();
            CHECK_REQUAL(T2.real(s1(i1),s2(i2),s3(i3)), R2.real(ci(j),s3(i3)));
            }
        }

    SECTION("One Index")
        {
        auto T1 = randIT(s4,b5,s1,l2);

        auto cs4 = combiner(s4);
        auto Rs4a = T1*cs4;
        CHECK(!hasindex(Rs4a,s4));
        CHECK(commonIndex(cs4,Rs4a));
        auto Rs4b = cs4*T1;
        CHECK(!hasindex(Rs4b,s4));
        CHECK(commonIndex(cs4,Rs4b));

        auto cl2 = combiner(l2);
        auto Rl2a = T1*cl2;
        CHECK(!hasindex(Rl2a,l2));
        CHECK(commonIndex(cl2,Rl2a));
        auto Rl2b = cl2*T1;
        CHECK(commonIndex(cl2,Rl2b));
        CHECK(!hasindex(Rl2b,l2));
        CHECK(hasindex(Rl2b,s4));
        CHECK(hasindex(Rl2b,b5));
        CHECK(hasindex(Rl2b,s1));
        }
    }

} //TEST_CASE("ITensor")


