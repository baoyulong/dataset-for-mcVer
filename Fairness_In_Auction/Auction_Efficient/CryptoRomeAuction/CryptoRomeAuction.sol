// Auction_0x760898e1e75dd7752db30bafa92d5f7d9e329a81

pragma solidity ^0.4.25;
contract CryptoRomeAuction {
    uint256 public auctionStart;
    uint256 public startingPrice;
    uint256 public endingPrice;
    uint256 public auctionEnd;
    uint256 public extensionTime;
    uint256 public highestBid;
    address payable public  highestBidder;
    bytes32 public highestBidderCC;
    bool public highestBidIsCC;
    address payable public  paymentAddress;
    uint256 amount;
    bool public ended;
    mapping(address=>uint) refunds;
    constructor(uint256 _startTime, uint256 _startingPrice, uint256 _duration, uint256 _extensionTime, address _wallet) public {
        require(_wallet != 0);
        require(_duration > 0);
        require(_duration >= _extensionTime);
        auctionStart = _startTime;
        startingPrice = _startingPrice;
        auctionEnd = auctionStart + _duration;
        extensionTime = _extensionTime;
        paymentAddress = _wallet;
        highestBid = 0;
    }

    function bid(uint block_timestamp) public payable{
        require(block_timestamp >= auctionStart);
        require(block_timestamp < auctionEnd);
        require(msg_value >= highestBid + 5);
        if (highestBid != 0) {
            refunds[highestBidder] += highestBid;
        }
        if (block_timestamp > auctionEnd - extensionTime) {
            auctionEnd = block_timestamp + extensionTime;
        }
        highestBidder = msg.sender;
        highestBid = msg.value;
        amount += msg.value;
        highestBidIsCC = false;
    }

    function endAuction() public onlyOwner {
        require(!ended);
        ended = true;
        if (!highestBidIsCC) {
            paymentAddress.transfer(amount);
        }
    }
}