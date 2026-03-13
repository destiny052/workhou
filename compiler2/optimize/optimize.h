#ifndef OPTIMIZE_H
#define OPTIMIZE_H

#include "ir/ir.h"

/* 对 IR 进行优化（当前阶段：仅遍历和打印） */
void optimize_ir(IRList* ir);

#endif
