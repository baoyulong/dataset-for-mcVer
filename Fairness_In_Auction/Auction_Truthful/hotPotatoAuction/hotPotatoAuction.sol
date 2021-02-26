// Auction_0x34eee0379a71e445b0dc52bda1d733c449ef1246
pragma solidity >=0.4.21;
contract hotPotatoAuction {
    uint256 public totalBids;
    uint256 public startingPrice;
    uint256 public currentBid;
    uint256 public currentMinBid;
    uint256 public auctionEnd;
    uint256 public hotPotatoPrize;
    address payable public seller;
    address payable public highBidder;
    address payable public loser;

    constructor(uint256 _startingPrice,uint256 _auctionEnd) public{
        startingPrice = _startingPrice;
        currentMinBid = _startingPrice;
        totalBids = 0;
        seller = msg.sender;
        auctionEnd = _auctionEnd;
        hotPotatoPrize = _startingPrice;
        currentBid = 0;
    }
    
    mapping(address => uint256) public balanceOf;

    function withdrawBalance(uint256 amount) public {
        require(amount <= address(this).balance);
        require (msg.sender == seller);
        seller.transfer(amount);
    }

    function bid(uint block_timestamp) public payable {
        require(block_timestamp < auctionEnd);
        require(msg.value >= startingPrice);
        require (msg.value >= currentMinBid);
        if(totalBids !=0){
            loser = highBidder;
            uint256 amount = balanceOf[loser];
            balanceOf[loser] = 0;
            require(loser.transfer(amount));
        }
        highBidder = msg.sender;
        currentBid = msg.value;
        hotPotatoPrize = currentBid/2;
        balanceOf[msg.sender] = msg.value + hotPotatoPrize;
        if(currentBid < 5){
            currentMinBid = msg.value + currentBid/2;
            hotPotatoPrize = currentBid/2; 
        }else{
            currentMinBid = msg.value + currentBid;
            hotPotatoPrize = currentBid/2;
        }
        totalBids = totalBids + 1;
    }

    function resolve() public {
        require(block_timestamp >= auctionEnd);
        require(msg.sender == seller);
        require (highBidder != address(0));
        
        // require (token.transfer(highBidder));

        balanceOf[seller] += balanceOf[highBidder];
        balanceOf[highBidder] =  0;
        highBidder = 0;
    }
}