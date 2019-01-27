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

      //Let's prepare array with content for the table:
      let rowsNum = response.length;
      let tableContent = new Array(rowsNum);
      for (let i = 0; i < rowsNum; i++) {
        let a = document.createElement('a');
        a.href = response[i].link.trim();
        a.target = "_blank";
        a.appendChild(document.createTextNode(response[i].title.trim()));
        tableContent[i] = [a, document.createTextNode(response[i].price.trim())]; //Link in 1st column, price in 2nd.
      }

      //Prepare array with header labels. Label are NOT JUST TEXT! They need to be DOM objects!
      let headerLabels = [document.createTextNode("Item name"), document.createTextNode("Price")];

      //And finally, generate our table:
      let tbl = createTable(tableContent, headerLabels, "sherlock-automatically-generated-table");

      //Append table as child element to div-container:
      currentMarketOutputBlock.appendChild(tbl);
    };

    ajax_request.eventResponse(ajax_request, {});
  }

})(jQuery);

/**
 * This function generates simple table with header row from specified data (tableContent, headerLabels).
 * tableContent is 2-D array with DOM Elements as values:
 * [
 *  0 => [0 => DOM_Obj_1cell_1row, 1 => DOM_Obj_2cell_1row, 2 => DOM_Obj_3cell_1row, ...], <- first row
 *  1 => [0 => DOM_Obj_1cell_2row, 1 => DOM_Obj_2cell_2row, 2 => DOM_Obj_3cell_2row, ...], <- second row
 * ];
 * headerLabels is 1-D array, also with DOM Elements as values (not just strings!);
 * [
 *  0 => DOM_Obj_headerlabel_1, 1 => DOM_Obj_headerlabel_2, ...
 * ];
 * tableClass and tableID are just strings;
 */
function createTable(tableContent, headerLabels, tableClass = "", tableID = "") {
  let tbl = document.createElement('table');
  tbl.classList.add(tableClass);
  tbl.id = tableID;

  //Create and fill container for header row (<thead></thead>):
  let thead = document.createElement('thead');
  let tr = document.createElement('tr');
  for (let i = 0; i < headerLabels.length; i++) {
    let th = document.createElement('th');
    th.appendChild(headerLabels[i]);
    tr.appendChild(th);
  }

  thead.appendChild(tr);
  tbl.appendChild(thead);

  //Create and fill container for main table content (<tbody></tbody>):
  let tbody = document.createElement('tbody');
  let rowsNum = tableContent.length; //Calculate how much rows will be in our table.
  for (let r = 0; r < rowsNum; r++) {
    tr = tbody.insertRow(); //Create new row and insert it to <tbody>;
    let colsNum = tableContent[r].length;
    for (let c = 0; c < colsNum; c++) {
      let td = tr.insertCell();
      td.appendChild(tableContent[r][c]);
    }
  }

  tbl.appendChild(tbody);
  return tbl;
}
