const fetch = require("node-fetch");

fetch("http://localhost:8081/getObject")
  .then((res) => res.json())
  .then((json) => console.log(json));
