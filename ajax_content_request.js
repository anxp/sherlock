(function($) {

  $(document).ready(function() {
    var settings = {
      url: Drupal.settings.basePath + "get_demo_content",
      submit: {},
    };
    var ajax = new Drupal.ajax(false, false, settings);
    ajax.eventResponse(ajax, {});

    //Request for _user_selected_ fleamarkets (we do this via ajax), and then fetch every market by getMarketOffers():
    var basePath = Drupal.settings.basePath;
    $.post(basePath + "sherlock/selected-markets", null, function (response) {
      getMarketOffers(response);
    }, "json");

  });

  function getMarketOffers(x) {
    console.log(x);
  }

})(jQuery);
