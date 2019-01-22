(function($) {

  $(document).ready(function() {
    //Make ajax-request for USER SELECTED fleamarkets (then fetch every market by getMarketOffers()):
    var request_settings = {
      url: Drupal.settings.basePath + "sherlock/selected-markets",
      submit: {},
    };

    var ajax_request = new Drupal.ajax(false, false, request_settings);
    ajax_request.options.success = function(response, status, xmlhttprequest) {
      console.log(typeof response);
      for(let i = 0; i < response.length; i++){
        console.log(response[i]);
        getMarketOffers(response[i]);
      }
    };

    ajax_request.eventResponse(ajax_request, {});
  });

  function getMarketOffers(selected_market) {
    var basePath = Drupal.settings.basePath;
    var request_settings = {
      url: Drupal.settings.basePath + "sherlock/market-fetch",
      submit: {
        market_id: selected_market,
      },
    };
    var ajax_request = new Drupal.ajax(false, false, request_settings);
    ajax_request.options.success = function(response, status, xmlhttprequest) {
      console.log(response);
    };
    ajax_request.eventResponse(ajax_request, {});
  }

})(jQuery);
