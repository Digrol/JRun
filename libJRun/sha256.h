

/**
 * Abstract cryptographic hash function.
 * Processes binary data and produces 'hash' (another byte array).
 * 1. Calculating hash in one step:
 *      Binary hash = new IHash().calc( data );
 * 2. Calculating hash by parts, using 'reset', 'process', 'getHash':
 *      IHash alg = new IHash();
 *      alg.process( part1 );
 *      alg.process( partN );
 *      Binary hash = alg.getHash();
 */


