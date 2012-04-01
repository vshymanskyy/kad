/**
 * @file
 * @brief SHA-1 computation class
 */

#ifndef ___SHA1_H___
#define ___SHA1_H___

#include <stdint.h>

/**
 * SHA-1 hash digest
 */

class SHA1
{

public:
	/**
	 * Default Constructor
	 */
	SHA1();

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
	 * Compute the SHA-1 hash key of a file
	 * @param szFileName The filename
	 * @return true on success false otherwise
	 */
	bool HashFile(const char* fn);

	/**
	 * Compute the SHA-1 hash key of a file
	 * @param szFileName The filename
	 * @return true on success false otherwise
	 */
	void HashString(const char* str);

	/**
	 * Finalize hash and report
	 */
	void Finalize(uint8_t digest[20]);

	/**
	 * Returns resulting hash length
	 */
	int GetHashLength() const
	{
		return 20;
	}

private:
	void Transform(const uint8_t buffer[64]);

private:
	uint8_t mWorkspace[64];
	uint32_t mState[5];
	uint32_t mCount[2];
	uint8_t mBuffer[64];
};

#endif // ___SHA1_H___
