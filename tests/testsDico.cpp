//
// Created by charl on 2024-04-14.
//

#include "gtest/gtest.h"
#include "DicoSynonymes.h"


using namespace TP3;


TEST(DicoSynonymes,simil)
{
    DicoSynonymes d1;
    std::cout << d1.similitude("AA", "AAA");
}

class DicoSynonymesTest : public ::testing::Test {
public:

    DicoSynonymesTest() : d1(), d2() {}

protected:

    DicoSynonymes d1;
    DicoSynonymes d2;

};

TEST_F(DicoSynonymesTest, ajouterRadical)
{
    d1.ajouterRadical("d");
    d1.ajouterRadical("a");
    d1.ajouterRadical("c");
    d1.ajouterRadical("b");
    int num = -1;
    d1.ajouterSynonyme("a","A", num);
    num = 0;
    d1.ajouterSynonyme("a","AA", num);
    std::cout << d1;

}