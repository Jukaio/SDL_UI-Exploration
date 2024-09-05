//
// Created by David Naussed on 2023-12-23.
//

#ifndef HIGHESTBITDEBRUIJN_IMPLEMENTATION_H
#define HIGHESTBITDEBRUIJN_IMPLEMENTATION_H

inline Uint32 HighestSetBitDeBruijn32(Uint32 v)
{
	static const Uint32 MultiplyDeBruijnBitPosition[32] = {0, 9,  1,  10, 13, 21, 2,  29, 11, 14, 16, 18, 22, 25, 3, 30,
														   8, 12, 20, 28, 15, 17, 24, 7,  19, 27, 23, 6,  26, 5,  4, 31};

	v |= v >> 1; // first round down to one less than a power of 2
	v |= v >> 2;
	v |= v >> 4;
	v |= v >> 8;
	v |= v >> 16;

	return MultiplyDeBruijnBitPosition[(v * 0x07C4ACDDU) >> 27];
}

#endif // HIGHESTBITDEBRUIJN_IMPLEMENTATION_H
