#ifndef KAD_CONFIG_H_
#define KAD_CONFIG_H_

/**
 * Kademlia ID size
 **/
#define KADEMLIA_ID_BITS 160
#define KADEMLIA_ID_SIZE (KADEMLIA_ID_BITS/8)

/**
 * The time after which a key/value pair expires
 * This is a time-to-live (TTL) from the original publication date
 *
 * Note: The fact that REPUBLISH and EXPIRE are equal introduces a race condition.
 * The STORE for the data being published may arrive at the node just after it has been expired,
 * so that it will actually be necessary to put the data on the wire.
 * A sensible implementation would have EXPIRE significantly longer than REPUBLISH.
 * Experience suggests that EXPIRE=86410 would be sufficient.
 **/
#define KADEMLIA_EXPIRE 86500

/**
 * The time after which an otherwise non-accessed bucket must be refreshed
 **/
#define KADEMLIA_REFRESH 3600

/**
 * The interval between Kademlia replication events, when a node is required to publish its entire database
 **/
#define KADEMLIA_REPLICATE 3600

/**
 * The time after which the original publisher
 * must republish a key/value pair
 **/
#define KADEMLIA_REPUBLISH 86400

/**
 * If no reply received from a node in this period (ms)
 * consider the node unresponsive.
 **/
#define KADEMLIA_TIMEOUT_RESPONCE 3000

/**
 * Timeout for performing an operation (ms)
 **/
#define KADEMLIA_TIMEOUT_OPERATION 10000

/**
 * Maximum number of concurrent messages in transit.
 **/
#define KADEMLIA_ALPHA 3

/**
 * K-Bucket size (K).
 **/
#define KADEMLIA_BUCKET_SIZE 20

/**
 * Size of replacement cache.
 **/
//#define KADEMLIA_CACHE_SIZE 7

/**
 * Number of times a node can be marked as stale before it is actually removed.
 **/
#define KADEMLIA_STALE 3

/**
 * This is the biggest message that won't split in a real network.
 **/
//#define KADEMLIA_MAX_MSG_SIZE 1400
#define KADEMLIA_MAX_MSG_SIZE 576 // RFC 791 (1981).

//#define KADEMLIA_CONTACT_INTERNAL

//#define KADEMLIA_DBG_BIN_ID

#endif /* KAD_CONFIG_H_ */
