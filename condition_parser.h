#pragma once

#include "node.h"

#include <memory>
#include <iostream>

shared_ptr<Node> ParseCondition(std::istream &is);

void TestParseCondition();
