/**
 * @file
 * @brief SHA-256 computation class
 */

#ifndef ___SHA256_H___
#define ___SHA256_H___

#include <stdint.h>
#include <stddef.h>

class SHA256
{

public:
	/**
	 * Default Constructor
	 */
	SHA256();

	/**
	 * Reset the internal state of the SHA-1 computation
	 * to compute a new SHA-1 hash key
	 */
	void Reset();

	/**
	 * Update the hash value from a byte buffer
	 * @param data the byte buffer
	 * @param len the number of byte available in the byte buffer
	 */
	void Update(const void* data, unsigned len);

	/**
	 * Finalize hash and report
	 */
	void Finalize(uint8_t digest[32]);

	/**
	 * Returns resulting hash length
	 */
	int GetHashLength() const
	{
		return 32;
	}

private:
	void Transform(const uint8_t buffer[64]);

private:
	uint32_t mTotal[2];
	uint32_t mState[8];
	uint8_t mBuffer[64];
};

#endif // ___SHA1_H___
