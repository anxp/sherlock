(function($) {

  $(document).ready(function() {
    //Make ajax-request for USER SELECTED fleamarkets (then fetch every market by getMarketOffers()):
    let request_settings = {
      url: Drupal.settings.basePath + "sherlock/selected-markets",
      submit: {},
    };

    let ajax_request = new Drupal.ajax(false, false, request_settings);
    ajax_request.options.success = function(response, status, xmlhttprequest) {
      //console.log(typeof response);
      for(let i = 0; i < response.length; i++){
        //console.log(response[i]);
        getMarketOffers(response[i]);
      }
    };

    ajax_request.eventResponse(ajax_request, {});
  });

  function getMarketOffers(selected_market) {
    let basePath = Drupal.settings.basePath;
    let request_settings = {
      url: Drupal.settings.basePath + "sherlock/market-fetch",
      submit: {
        market_id: selected_market,
      },
    };
    let ajax_request = new Drupal.ajax(false, false, request_settings);
    ajax_request.options.success = function(response, status, xmlhttprequest) {
      let currentMarketOutputBlock = document.getElementById(selected_market + '-output-block');
      let tbl = generateTable(response);
      currentMarketOutputBlock.appendChild(tbl);
    };

    ajax_request.eventResponse(ajax_request, {});
  }

})(jQuery);

function generateTable(tableContent) {
  let tbl = document.createElement('table');
  tbl.classList.add("sherlock-automatically-generated-table");

  let thead = document.createElement('thead');
  let tr = document.createElement('tr');

  let th = document.createElement('th');
  th.appendChild(document.createTextNode("Item name"));
  tr.appendChild(th);
  th = null;

  th = document.createElement('th');
  th.appendChild(document.createTextNode("Price"));
  tr.appendChild(th);

  thead.appendChild(tr);
  tbl.appendChild(thead);

  let tbody = document.createElement('tbody');

  for(let i = 0; i < tableContent.length; i++) {
    tr = tbody.insertRow();
    for(let j = 0; j < 2; j++) {
      if (j === 0) {
        let td = tr.insertCell();
        let a = document.createElement('a');
        a.href = tableContent[i].link.trim();
        a.target = "_blank";
        a.appendChild(document.createTextNode(tableContent[i].title.trim()));
        td.appendChild(a);
      }
      if (j === 1) {
        let td = tr.insertCell();
        td.appendChild(document.createTextNode(tableContent[i].price.trim()));
      }
    }
  }

  tbl.appendChild(tbody);

  return tbl;
}