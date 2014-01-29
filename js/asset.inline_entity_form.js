(function($) {

Drupal.behaviors.asset_inline_entity_form = {
  attach: function (context, settings) {
    var self = this;
    // $('.field-widget-inline-entity-form .asset-select').once(function(){
    //   $(this).click( self.assetSelect ).closest('td').wrapInner('<div class="asset" />');
    // });
    $('.asset-inline-entity-browser .asset-select').once().click( self.assetSelect );
  },

  assetSelect: function ( e ) {
    var self = this;
    e.preventDefault();
    var $this = $(this);
    var id = $this.attr('data-id');
    var $wrapper = $this.closest('.fieldset-wrapper');
    var parents = $('.parents',$wrapper).val();
    $('.asset-inline-entity',$wrapper).val('Asset (' + id + ')');
    $('input.ief-entity-submit',$wrapper).trigger('mousedown');
  }
}

})(jQuery);
