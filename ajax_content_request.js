(function($) {

  $(document).ready(function() {
    var settings = {
      url: Drupal.settings.basePath + "get_demo_content",
      submit: {},
    };
    var ajax = new Drupal.ajax(false, false, settings);
    ajax.eventResponse(ajax, {});
  });

})(jQuery);
