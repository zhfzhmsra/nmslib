/**
 * Non-metric Space Library
 *
 * Authors: Bilegsaikhan Naidan (https://github.com/bileg), Leonid Boytsov (http://boytsov.info).
 * With contributions from Lawrence Cayton (http://lcayton.com/) and others.
 *
 * For the complete list of contributors and further details see:
 * https://github.com/searchivarius/NonMetricSpaceLib 
 * 
 * Copyright (c) 2014
 *
 * This code is released under the
 * Apache License Version 2.0 http://www.apache.org/licenses/.
 *
 */

#include <algorithm>
#include <cstdint>

#include "distcomp.h"
#include "string.h"
#include "utils.h"

namespace similarity {

/* 
 * The maximum number of elements that will be kept on the stack
 * by the function levenshtein. 
 *
 * TODO:@leo If there are too many threads, we might run out stack memory.
 *           but it is probably extremely unlikely with the buffer of this size.
 *
 */

#define MAX_BUFFER_QTY  512

using namespace std;

template <class T> int levenshtein(const T* p1, size_t len1, const T* p2, size_t len2) {
  int aStackBuf[2 * MAX_BUFFER_QTY];
  int *pMemBuf = NULL;
  int *pBuff;

  if (len1 > len2) {
    swap(p1, p2);
    swap(len1, len2);
  }

  size_t nr = len1, nc = len2;

  if (!nr || !nc) return nr + nc;

  if (nr + 1 > MAX_BUFFER_QTY) {
    pBuff = pMemBuf = new int[2 * (nr + 1)];
  } else {
    pBuff = aStackBuf;
  }

  int* pColPtr[2] = {pBuff, pBuff + nr + 1};

  for (size_t k = 0; k <= nr; ++k) pColPtr[0][k] = k;

  size_t nCurr = 1;

  for (size_t i = 0; i < nc; ++i) {
    int* pPrevCol = pColPtr[1 - nCurr];
    int* pCurrCol = pColPtr[nCurr];

    pCurrCol[0] = i + 1;

    for (size_t k = 1; k <= nr; ++k) {
      int nextVal = min(1 + min(pPrevCol[k], pCurrCol[k-1]),  // insertion or deletion
          pPrevCol[k-1] + (p1[k-1]==p2[i] ? 0 : 1)); // substitution
      pCurrCol[k] = nextVal;
    }

    nCurr = 1 - nCurr;
  }

  // Memorize this value before removing memory
  int res = pColPtr[1 - nCurr][nr];

  if (pMemBuf) delete [] pMemBuf;

  return res;
}

template int levenshtein<char>(const char* p1, size_t len1, const char* p2, size_t len2);
template int levenshtein<char32_t>(const char32_t* p1, size_t len1, const char32_t* p2, size_t len2);

}
