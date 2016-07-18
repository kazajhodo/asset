(function($) {

Drupal.behaviors.asset_widget_display = {
  attach: function (context, settings) {
    var self = this;
    $('body').once('asset-full').each(function(){
      $(window).on('resize', function(){
        $('.asset-full:not(.cke_widget_element)').each(function(){
          self.align($(this));
        });
      });
    });
    $('.asset-full:not(.cke_widget_element)', context).once('asset-full').each(function(){
      self.align($(this));
    });
  },

  align: function ($element) {
    $element.css({marginLeft:'',marginRight:''});
    var maxWidth = $element.width();
    $element.css({marginLeft:'auto',marginRight:'auto'});
    $('.asset-reset', $element).each(function(){
      $(this).css({maxWidth:maxWidth, marginLeft:'auto', marginRight:'auto'});
    });
    var offset = $element.offset();
    $element.css({marginLeft:(offset.left * -1) + 'px',marginRight:(offset.left * -1) + 'px'});
  }
}

})(jQuery);
