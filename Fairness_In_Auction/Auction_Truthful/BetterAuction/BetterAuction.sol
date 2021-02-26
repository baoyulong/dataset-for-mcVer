pragma solidity >=0.4.8;

contract BetterAuction {
    // Auction start time, seconds from 1970-01-01
    uint256 public auctionStart;
    // Auction bidding period in seconds, relative to auctionStart
    uint256 public biddingPeriod;

    // Address of the highest bidder
    address public highestBidder;
    // Highest bid amount
    uint256 public highestBid;
    // Allowed withdrawals of previous bids
    mapping(address => uint256) pendingReturns;
    // Set to true at the end, disallows any change
    bool auctionClosed;
 
    // modifier isAuctionActive {
    //     require(block_timestamp >= auctionStart || block_timestamp <= (auctionStart + biddingPeriod));
    //     _;
    // }
 
    // modifier isAuctionEnded {
    //     require(block_timestamp >= (auctionStart + biddingPeriod));
    //     _;
    // }

    constructor(uint block_timestamp,uint _biddingPeriod) public {
        auctionStart = block_timestamp;
        biddingPeriod = _biddingPeriod;
    }


    function bid(uint block_timestamp) payable public {
        require(block_timestamp >= auctionStart);
        require(block_timestamp <= auctionStart + biddingPeriod);
        if(msg.sender == highestBidder){
            highestBid += msg.value;
        }
        if(msg.sender!=highestBidder){
            uint256 amount = pendingReturns[msg.sender] + msg.value;
            if(amount > highestBid){
                pendingReturns[msg.sender] = 0;
                // Save previous highest bidders funds
                pendingReturns[highestBidder] = highestBid;
                // Record the highest bid
                highestBid = amount;
                highestBidder = msg.sender;
            }
        }
    }
}