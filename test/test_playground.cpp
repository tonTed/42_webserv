//
// Created by Teddy BLANCO on 2023-03-16.
//

#include "test_header.hpp"
#include <string>

TEST_CASE("webServ Playground Test") {

	MESSAGE("random message");

	CHECK(true);

	CHECK_MESSAGE(false, "This should fail");

	CHECK_FALSE_MESSAGE(false, "should be false");
}

TEST_CASE("Abuse of Test Case!") {

	CHECK_EQ("is funny", "is funny");

	CHECK_UNARY_FALSE(0);
}