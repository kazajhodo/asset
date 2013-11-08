/**
 * @file
 * An overwrite of some core Views javascript.
 */
(function ($) {
/**
 * Javascript object for a certain view.
 */
if(Drupal.views && Drupal.views.assetAjaxView){
  Drupal.views.assetAjaxView = function(settings) {
    var selector = '.view-dom-id-' + settings.view_dom_id;
    this.$view = $(selector);

    if(!this.$view.length) return;
    // Remove already processed views
    delete Drupal.settings.views.ajaxViews['views_dom_id:' + settings.view_dom_id];

    // Retrieve the path to use for views' ajax.
    var ajax_path = Drupal.settings.views.ajax_path;

    // If there are multiple views this might've ended up showing up multiple times.
    if (ajax_path.constructor.toString().indexOf("Array") != -1) {
      ajax_path = ajax_path[0];
    }

    // Check if there are any GET parameters to send to views.
    var queryString = window.location.search || '';
    if (queryString !== '') {
      // Remove the question mark and Drupal path component if any.
      var queryString = queryString.slice(1).replace(/q=[^&]+&?|&?render=[^&]+/, '');
      if (queryString !== '') {
        // If there is a '?' in ajax_path, clean url are on and & should be used to add parameters.
        queryString = ((/\?/.test(ajax_path)) ? '&' : '?') + queryString;
      }
    }

    this.element_settings = {
      url: ajax_path + queryString,
      submit: settings,
      setClick: true,
      event: 'click',
      selector: selector,
      progress: { type: 'throbber' }
    };

    this.settings = settings;

    // Add the ajax to exposed forms.
    this.$exposed_form = $('form#views-exposed-form-'+ settings.view_name.replace(/_/g, '-') + '-' + settings.view_display_id.replace(/_/g, '-'));
    this.$exposed_form.once(jQuery.proxy(this.attachExposedFormAjax, this));

    // console.log(this.$exposed_form);
    // console.log(this.attachPagerAjax);

    // Add the ajax to pagers.
    this.$view
      // Don't attach to nested views. Doing so would attach multiple behaviors
      // to a given element.
      .filter(jQuery.proxy(this.filterNestedViews, this))
      .once(jQuery.proxy(this.attachPagerAjax, this));
  };

  Drupal.views.assetAjaxView.prototype.attachExposedFormAjax = Drupal.views.ajaxView.prototype.attachExposedFormAjax;
  Drupal.views.assetAjaxView.prototype.filterNestedViews = Drupal.views.ajaxView.prototype.filterNestedViews;
  Drupal.views.assetAjaxView.prototype.attachPagerAjax = Drupal.views.ajaxView.prototype.attachPagerAjax;
  Drupal.views.assetAjaxView.prototype.attachPagerLinkAjax = Drupal.views.ajaxView.prototype.attachPagerLinkAjax;

  Drupal.views.ajaxView = Drupal.views.assetAjaxView;
}

})(jQuery);
