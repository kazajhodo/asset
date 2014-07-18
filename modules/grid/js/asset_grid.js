(function($) {

Drupal.behaviors.asset_grid = {
  attach: function (context, settings) {
    var self = this;
    self.hideExtras(context, settings);
  },

  hideExtras: function (context, settings) {
    var $form = $('form.has-asset-grid');
    if($form.length){
      $('.form-actions,.form-submit', $form).show();
      $('.form-actions:not(:first)', $form).hide();
      if($('.ief-entity-submit', $form).length){
        $('#edit-submit', $form).hide();
      }
    }
  }
}

})(jQuery);
