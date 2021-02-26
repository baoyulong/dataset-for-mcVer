// Auction_0x6c1c2fd38fccc0b010f75b2ece535cf57543ddcd
pragma ^0.4.18;
contract Rewrite{
    bool initialPrice;
    uint256 timestampEnd;
    uint256 price;
    address winner;
    mapping(address=>uint256) bids;
    uint256 timeBound=2;
    uint256  increaseTimeBy=1;
    constructor(uint _timestampEnd, uint _price){
      timestampEnd = _timestampEnd;
      initialPrice = true;
      price = _price;
    }
    function bid(uint256 block_timestamp) public payable {
      require(block_timestamp < timestampEnd);
      if (bids[msg.sender] > 0) {
        bids[msg.sender] += msg.value;
      }
      if (bids[msg.sender] <= 0) {
        bids[msg.sender] = msg.value;
      }
      if (initialPrice) {
        require(bids[msg.sender] >= price);
      } 
      if(!initialPrice) {
        require(bids[msg.sender] >= price +2 );
      }
      
      if (block_timestamp > timestampEnd - timeBound) {
        timestampEnd = block_timestamp + increaseTimeBy;
      }
      initialPrice = false;
      price= bids[msg.sender];
      winner = msg.sender;
  }
}