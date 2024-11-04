
#include "stdinc.h"

#if 0

// ---------------------------------------------------------------------------------------------------------------------
typedef void FUNCT_TRANSFORM( SHA256_CTX* ctx, const uint8_t data[] );

FUNCT_TRANSFORM *do_transform;

// ---------------------------------------------------------------------------------------------------------------------
static const uint32_t SHA256_CONST[64] = {
	0x428a2f98,0x71374491,0xb5c0fbcf,0xe9b5dba5,0x3956c25b,0x59f111f1,0x923f82a4,0xab1c5ed5,
	0xd807aa98,0x12835b01,0x243185be,0x550c7dc3,0x72be5d74,0x80deb1fe,0x9bdc06a7,0xc19bf174,
	0xe49b69c1,0xefbe4786,0x0fc19dc6,0x240ca1cc,0x2de92c6f,0x4a7484aa,0x5cb0a9dc,0x76f988da,
	0x983e5152,0xa831c66d,0xb00327c8,0xbf597fc7,0xc6e00bf3,0xd5a79147,0x06ca6351,0x14292967,
	0x27b70a85,0x2e1b2138,0x4d2c6dfc,0x53380d13,0x650a7354,0x766a0abb,0x81c2c92e,0x92722c85,
	0xa2bfe8a1,0xa81a664b,0xc24b8b70,0xc76c51a3,0xd192e819,0xd6990624,0xf40e3585,0x106aa070,
	0x19a4c116,0x1e376c08,0x2748774c,0x34b0bcb5,0x391c0cb3,0x4ed8aa4a,0x5b9cca4f,0x682e6ff3,
	0x748f82ee,0x78a5636f,0x84c87814,0x8cc70208,0x90befffa,0xa4506ceb,0xbef9a3f7,0xc67178f2
};

// ---------------------------------------------------------------------------------------------------------------------
void sha256_transform( SHA256_CTX* ctx, const uint8_t data[] )
{
	uint32_t a = ctx->state[ 0 ];
	uint32_t b = ctx->state[ 1 ];
	uint32_t c = ctx->state[ 2 ];
	uint32_t d = ctx->state[ 3 ];
	uint32_t e = ctx->state[ 4 ];
	uint32_t f = ctx->state[ 5 ];
	uint32_t g = ctx->state[ 6 ];
	uint32_t h = ctx->state[ 7 ];

	uint32_t t1;
	uint8_t i;

	uint32_t W[16];

	for( i = 0; i < 16; ++i )
	{
		W[ i ] = READ_U32( data );
		data += sizeof(uint32_t);
	}

	for( i = 0; i < 16; ++i )
	{
		t1 = h + (ROTR_U32(e, 6) ^ ROTR_U32(e, 11) ^ ROTR_U32(e, 25))
			+ ((e & f) ^ (~e & g)) + SHA256_CONST[ i ] + W[ i ];
		h = g;
		g = f;
		f = e;
		e = d + t1;
		d = c;
		c = b;
		b = a;
		a = t1 + (ROTR_U32(a, 2) ^ ROTR_U32(a, 13) ^ ROTR_U32(a, 22))  +  ((a & c) ^ (a & d) ^ (c & d));
	}

	for( ; i < 64; ++i )
	{
		W[ i & 0x0F ] += W[(i - 7) & 0x0F]
			+ (ROTR_U32( W[(i - 15) & 0x0F], 7 )  ^ ROTR_U32( W[(i - 15) & 0x0F], 18 ) ^ (W[(i - 15) & 0x0F] >> 3))
			+ (ROTR_U32( W[(i - 2)  & 0x0F], 17 ) ^ ROTR_U32( W[(i - 2)  & 0x0F], 19 ) ^ (W[(i - 2)  & 0x0F] >> 10));

		t1 = h + (ROTR_U32( e, 6 ) ^ ROTR_U32( e, 11 ) ^ ROTR_U32( e, 25 ))
			+ ((e & f) ^ (~e & g)) + SHA256_CONST[ i ] + W[ i & 0x0F ];
		h = g;
		g = f;
		f = e;
		e = d + t1;
		d = c;
		c = b;
		b = a;
		a = t1 + (ROTR_U32(a, 2) ^ ROTR_U32(a, 13) ^ ROTR_U32(a, 22))  +  ((a & c) ^ (a & d) ^ (c & d));
	}

	ctx->state[0] += a;
	ctx->state[1] += b;
	ctx->state[2] += c;
	ctx->state[3] += d;
	ctx->state[4] += e;
	ctx->state[5] += f;
	ctx->state[6] += g;
	ctx->state[7] += h;
}

// ---------------------------------------------------------------------------------------------------------------------
void sha256_init( SHA256_CTX* ctx )
{
	memset( ctx, 0, sizeof( SHA256_CTX ) );
	ctx->state[0] = 0x6a09e667;
	ctx->state[1] = 0xbb67ae85;
	ctx->state[2] = 0x3c6ef372;
	ctx->state[3] = 0xa54ff53a;
	ctx->state[4] = 0x510e527f;
	ctx->state[5] = 0x9b05688c;
	ctx->state[6] = 0x1f83d9ab;
	ctx->state[7] = 0x5be0cd19;
	do_transform = sha256_transform;
}

// ---------------------------------------------------------------------------------------------------------------------
void sha224_init( SHA256_CTX* ctx )
{
	memset( ctx, 0, sizeof( SHA256_CTX ) );
	ctx->state[0] = 0xC1059ED8;
	ctx->state[1] = 0x367CD507;
	ctx->state[2] = 0x3070DD17;
	ctx->state[3] = 0xF70E5939;
	ctx->state[4] = 0xFFC00B31;
	ctx->state[5] = 0x68581511;
	ctx->state[6] = 0x64F98FA7;
	ctx->state[7] = 0xBEFA4FA4;
	do_transform = sha256_transform;
}

// ---------------------------------------------------------------------------------------------------------------------
BoolSecure sha_set_state( SHA256_CTX* ctx, const uint8_t* state, uint16_t stateLen, uint32_t processedLen[ 2 ] )
{
	uint16_t i;
	if( (stateLen != 20) && (stateLen != 32) )
		return SECURE_FALSE_8;

	ctx->total[ 0 ] = processedLen[ 0 ];
	ctx->total[ 1 ] = processedLen[ 1 ];

	stateLen >>= 2;
	for( i = 0; i < stateLen; ++i )
	{
		ctx->state[ i ] = READ_U32( state );
		state += sizeof(uint32_t);
	}

	return SECURE_TRUE_8;
}

// ---------------------------------------------------------------------------------------------------------------------
void sha_update( SHA256_CTX* ctx, const uint8_t msg[], uint32_t msgLen )
{
	uint8_t fill;
	uint32_t left;

	if( msgLen == 0 )
		return;

	left = ctx->total[ 0 ] & 0x3F;
	fill = 64 - left;

	ctx->total[ 0 ] += msgLen;

	if( ctx->total[ 0 ] < msgLen )
		ctx->total[ 1 ]++;

	if( left && (msgLen >= fill) )
	{
		memcpy( ctx->data + left, msg, fill );
		do_transform( ctx, ctx->data );
		msg += fill;
		msgLen -= fill;
		left = 0;
	}

	while( msgLen >= 64 )
	{
		do_transform( ctx, msg );
		msg += 64;
		msgLen -= 64;
	}

	if( msgLen > 0 )
		memcpy( ctx->data + left, msg, msgLen );
}

// ---------------------------------------------------------------------------------------------------------------------
void sha_final( SHA256_CTX* ctx )
{
	uint32_t leftBytes = ctx->total[0] & 0x3F;
	uint32_t i = leftBytes;

	// Pad remaining data
	ctx->data[i++] = 0x80;
	memset(ctx->data + i, 0, 64 - i);
	if ( leftBytes >= (64 - 8))
	{
		do_transform(ctx, ctx->data );
		memset(ctx->data, 0, 56);
	}

	// Total len in bits, BigEndian
	writeU32( ctx->data + 56, (ctx->total[ 0 ] >> 29) | (ctx->total[ 1 ] << 3) );
	writeU32( ctx->data + 60, (ctx->total[ 0 ] << 3) );
	do_transform( ctx, ctx->data );

	for( i = 0; i < sizeof(ctx->state); i += 4 )
		writeU32( ctx->data + i, ctx->state[ i >> 2 ] );
}

// ---------------------------------------------------------------------------------------------------------------------
#define SHA1_DATA_WORD( i ) \
	t = W[ (i-3) & 0x0F ] ^ W[ (i-8) & 0x0F ] ^ W[ (i-14) & 0x0F ] ^ W[ (i-16) & 0x0F ];\
	W[ i & 0x0F ] = ROTL_U32( t, 1 );

// ---------------------------------------------------------------------------------------------------------------------
void sha1_transform( SHA256_CTX *ctx, const uint8_t data[] )
{
	uint32_t a = ctx->state[0];
	uint32_t b = ctx->state[1];
	uint32_t c = ctx->state[2];
	uint32_t d = ctx->state[3];
	uint32_t e = ctx->state[4];

	uint32_t i;
	uint32_t t;
	uint32_t W[ 16 ];

	for( i = 0; i < 16; ++i )
	{
		W[ i ] = READ_U32( data );
		data += sizeof(uint32_t);
	}

	for( i = 0; i < 16; ++i )
	{
		t = ROTL_U32( a, 5 ) + ((b & c) ^ (~b & d)) + e + 0x5a827999 + W[ i ];
		e = d;
		d = c;
		c = ROTL_U32( b, 30 );
		b = a;
		a = t;
	}

	for( ; i < 20; ++i )
	{
		SHA1_DATA_WORD( i );
		t = ROTL_U32( a, 5 ) + ((b & c) ^ (~b & d)) + e + 0x5a827999 + W[ i & 0x0F ];
		e = d;
		d = c;
		c = ROTL_U32( b, 30 );
		b = a;
		a = t;
	}

	for ( ; i < 40; ++i)
	{
		SHA1_DATA_WORD( i );
		t = ROTL_U32(a, 5) + (b ^ c ^ d) + e + 0x6ed9eba1 + W[ i & 0x0F ];
		e = d;
		d = c;
		c = ROTL_U32(b, 30);
		b = a;
		a = t;
	}

	for ( ; i < 60; ++i)
	{
		SHA1_DATA_WORD( i );
		t = ROTL_U32(a, 5) + ((b & c) ^ (b & d) ^ (c & d))  + e + 0x8f1bbcdc + W[ i & 0x0F ];
		e = d;
		d = c;
		c = ROTL_U32(b, 30);
		b = a;
		a = t;
	}

	for ( ; i < 80; ++i)
	{
		SHA1_DATA_WORD( i );
		t = ROTL_U32(a, 5) + (b ^ c ^ d) + e + 0xca62c1d6 + W[ i & 0x0F ];
		e = d;
		d = c;
		c = ROTL_U32(b, 30);
		b = a;
		a = t;
	}

	ctx->state[0] += a;
	ctx->state[1] += b;
	ctx->state[2] += c;
	ctx->state[3] += d;
	ctx->state[4] += e;
}

// ---------------------------------------------------------------------------------------------------------------------
void sha1_init( SHA256_CTX *ctx )
{
	memset( ctx, 0, sizeof(SHA256_CTX) );
	ctx->state[0] = 0x67452301;
	ctx->state[1] = 0xEFCDAB89;
	ctx->state[2] = 0x98BADCFE;
	ctx->state[3] = 0x10325476;
	ctx->state[4] = 0xc3d2e1f0;
	do_transform = sha1_transform;
}

// ---------------------------------------------------------------------------------------------------------------------
uint16_t sha_getCtxSize()
{
	return sizeof( SHA256_CTX );
}

// ---------------------------------------------------------------------------------------------------------------------
void calcHashSHA1( const uint8_t* data, uint32_t length, uint8_t* bufHash )
{
	SHA256_CTX ctx;
	sha1_init( &ctx );
	sha_update( &ctx, data, length );
	sha_final( &ctx );
	memcpy( bufHash, ctx.data, HASH_SIZE_SHA1 );
}

// ---------------------------------------------------------------------------------------------------------------------
void calcHashSHA256( const uint8_t* data, uint32_t length, uint8_t* bufHash )
{
	SHA256_CTX ctx;
	calcHashSHA256CTX( &ctx, data, length, bufHash );
}

// ---------------------------------------------------------------------------------------------------------------------
void calcHashSHA256CTX( SHA256_CTX* ctx, const uint8_t* data, uint32_t length, uint8_t* bufHash )
{
	sha256_init( ctx );
	sha_update( ctx, data, length );
	sha_final( ctx );
	memcpy( bufHash, ctx->data, HASH_SIZE_SHA256 );
}

#endif
